/* Copyright (c) 1997-2015
   Ewgenij Gawrilow, Michael Joswig (Technische Universitaet Berlin, Germany)
   http://www.polymake.org

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version: http://www.gnu.org/licenses/gpl.txt.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
--------------------------------------------------------------------------------
*/

#include "polymake/perl/Ext.h"

static HV *string_stash, *integer_stash, *float_stash, *UNIVERSAL_stash;

static MGVTBL pkg_retrieval_index_vtbl={ 0, 0, 0, 0, 0 };
static MGVTBL stored_kw_vtbl={ 0, 0, 0, 0, 0 };

#if PerlVersion < 5101
# define mro_meta_init(s) Perl_mro_meta_init(aTHX_ s)

struct mro_alg {
    const char *name;
    AV *(*resolve)(pTHX_ HV* stash, I32 level);
};
#endif

static inline
HV* pkg_of_object(pTHX_ SV* obj, I32 try_repeated, I32* bundled_repeated)
{
   MAGIC* mg;
   if (!SvOBJECT(obj)) {
      if (try_repeated) {
         // anonymous array of objects?
         if (SvTYPE(obj)==SVt_PVAV && AvFILLp(obj)>=0 &&
             (obj=AvARRAY(obj)[0], SvROK(obj)) &&
             (obj=SvRV(obj), SvOBJECT(obj))) {
            *bundled_repeated=TRUE;
         } else {
            return UNIVERSAL_stash;
         }
      } else {
         return UNIVERSAL_stash;
      }
   }
   mg=mg_findext(obj, PERL_MAGIC_ext, &pkg_retrieval_index_vtbl);
   return mg ? gv_stashsv(AvARRAY(obj)[mg->mg_private], FALSE) : SvSTASH(obj);
}

MODULE = Polymake::Overload             PACKAGE = Polymake::Overload

PROTOTYPES: DISABLE

void
is_keyword_or_hash(sv)
   SV *sv;
PPCODE:
{
   if (SvROK(sv) ? (sv=SvRV(sv), SvTYPE(sv) == SVt_PVHV && SvSTASH(sv)==NULL)
                 : (SvFLAGS(sv) & (SVf_POK|SVf_IVisUV)) == (SVf_POK|SVf_IVisUV))
      PUSHs(&PL_sv_yes);
   else
      PUSHs(&PL_sv_no);
}

void
can_signature(arg, signature, try_repeated_sv)
   SV* arg;
   SV* signature;
   SV* try_repeated_sv;
PPCODE:
{
   HV* stash;
   STRLEN method_name_len;
   char* method_name=SvPV(signature, method_name_len);
   I32 try_repeated=SvIVX(try_repeated_sv);
   I32 bundled_repeated=FALSE;
   stash=SvROK(arg)
         ? pkg_of_object(aTHX_ SvRV(arg), try_repeated, &bundled_repeated) :
         SvIOK(arg)
         ? integer_stash :
         SvNOK(arg)
         ? float_stash :
         SvPOK(arg)
         ? string_stash
         : UNIVERSAL_stash;
   GV* glob=gv_fetchmeth(stash, method_name, method_name_len, 0);
   if (!glob && stash==string_stash) {
      // maybe this is a number stored as a string?
      I32 num_flags=looks_like_number(arg);
      if (num_flags & IS_NUMBER_IN_UV)
         glob=gv_fetchmeth(integer_stash, method_name, method_name_len, 0);
      else if (num_flags & IS_NUMBER_NOT_INT)
         glob=gv_fetchmeth(float_stash, method_name, method_name_len, 0);
   }
   if (glob) {
      dTARGET;
      if (SvROK(TARG)) {
         SvREFCNT_dec(SvRV(TARG));
      } else {
         SvUPGRADE(TARG, SVt_RV);
         SvROK_on(TARG);
      }
      SvRV_set(TARG, SvREFCNT_inc_simple_NN((SV*)GvCV(glob)));
      if (try_repeated) SvIV_set(try_repeated_sv, bundled_repeated);
      PUSHs(try_repeated_sv);
      PUSHs(TARG);
   }
}

void
can_next(arg, nodesubref)
  SV* arg;
  SV* nodesubref;
PPCODE:
{
   /* This is a simplified version of mro::next::can, with following changes:
    *  - The subroutine marking the `current' package is passed explicitly
    *    rather than deduced from the call context stack;
    *  - UNIVERSAL package is considered as well (get_linear_isa never appends it)
    *  - The central nextmethod cache is not used; instead, each subroutine provides its own cache in a state variable.
    *
    * Unfortunately, the current implementation of mro__nextcan is monolithic,
    * so it's impossible to reuse any parts of it without dumb copying code.
    */
   I32 _bundle_repeated;
   HV* arg_pkg=SvROK(arg) ? pkg_of_object(aTHX_ SvRV(arg), TRUE, &_bundle_repeated) : UNIVERSAL_stash;
   CV* nodesub=(CV*)SvRV(nodesubref);
   GV* nodesub_gv=CvGV(nodesub);
   const char* subname=GvNAME(nodesub_gv);
   STRLEN subname_len=GvNAMELEN(nodesub_gv);
   PADLIST* nodesub_padlist=CvPADLIST(nodesub);
   SV* next_method_cache=(HV*)PAD_BASE_SV(nodesub_padlist, 1);
   HE* next_method_entry;
   SV* next_method_ref;
   GV* candidate;
   CV* cand_cv;
   GV** gvp;
   SV fake_stash_ref={ NULL, 1, SVt_RV | SVf_ROK };
   assert(next_method_cache && SvTYPE(next_method_cache)==SVt_PVHV);
   SvRV_set(&fake_stash_ref, (SV*)arg_pkg);
   next_method_entry=pm_perl_refhash_fetch_ent(aTHX_ (HV*)next_method_cache, &fake_stash_ref, TRUE);
   next_method_ref=HeVAL(next_method_entry);

   if (SvOK(next_method_ref)) {
      // already cached
      if (SvROK(next_method_ref)) {
         PUSHs(next_method_ref);
      } else {
         PUSHs(&PL_sv_undef);
      }
      XSRETURN(1);
   }

   if (arg_pkg != UNIVERSAL_stash) {
      HV* nodesub_stash=GvSTASH(nodesub_gv);
      const char* nodesub_stash_name=HvNAME(nodesub_stash);
      STRLEN nodesub_stash_namelen=HvNAMELEN_get(nodesub_stash);

      // *** this part is "borrowed" from ext/mro/mro.xs ***

      /* has ourselves at the top of the list */
      struct mro_meta* arg_pkg_mro_meta = HvMROMETA(arg_pkg);
      AV* linear_av = arg_pkg_mro_meta->mro_which->resolve(aTHX_ arg_pkg, 0);
      SV** linear_svp = AvARRAY(linear_av);
      I32 entries = AvFILLp(linear_av) + 1;

      /* Walk down our MRO, skipping everything up
         to the contextually enclosing class */
      while (entries--) {
         SV * const linear_sv = *linear_svp++;
         assert(linear_sv);
         if (SvCUR(linear_sv)==nodesub_stash_namelen && strEQ(SvPVX(linear_sv), nodesub_stash_name))
            break;
      }

      /* Now search the remainder of the MRO for the
         same method name as the contextually enclosing
         method */
      if (entries > 0) {
         while (entries--) {
            SV * const linear_sv = *linear_svp++;
            HV* curstash;

            assert(linear_sv);
            curstash = gv_stashsv(linear_sv, FALSE);
            assert(curstash);

            gvp = (GV**)hv_fetch(curstash, subname, subname_len, 0);
            if (gvp &&
                (candidate = *gvp, SvTYPE(candidate) == SVt_PVGV) &&
                (cand_cv = GvCV(candidate)) &&
                !GvCVGEN(candidate)) {

               /* Notably, we only look for real entries, not method cache
                  entries, because in C3 the method cache of a parent is not
                  valid for the child */
               SvUPGRADE(next_method_ref, SVt_RV);
               SvROK_on(next_method_ref);
               SvRV_set(next_method_ref, SvREFCNT_inc_simple_NN((SV*)cand_cv));
               PUSHs(next_method_ref);
               XSRETURN(1);
            }
         }
      }
   }

   gvp = (GV**)hv_fetch(UNIVERSAL_stash, subname, subname_len, 0);
   if (gvp &&
       (candidate = *gvp, SvTYPE(candidate) == SVt_PVGV) &&
       (cand_cv = GvCV(candidate)) &&
       !GvCVGEN(candidate)) {

      SvUPGRADE(next_method_ref, SVt_RV);
      SvROK_on(next_method_ref);
      SvRV_set(next_method_ref, SvREFCNT_inc_simple_NN((SV*)cand_cv));
      PUSHs(next_method_ref);
      XSRETURN(1);
   }

   // store the failure flag in the cache
   sv_setiv(next_method_ref, 0);
   XSRETURN_UNDEF;
}


void
store_kw_args(args, first)
   AV* args=(AV*)SvRV(ST(0));
   I32 first;
CODE:
{
   /* Equivalent to
        my @kw_args=splice @args, $first;
        attach_magic(\@args, \@kw_args);
      but without touching REAL flags of the arrays and refcounts of the elements
   */
   I32 n_args=AvFILLp(args)+1-first;
   AV* kw_args=newAV();
   SV* kw_ref=newRV_noinc((SV*)kw_args);
   SV** kw_start=AvARRAY(args)+first;
   SV** kw_last=AvARRAY(args)+AvFILLp(args);
   av_fill(kw_args, n_args-1);
   if (!AvREAL(args)) AvREAL_off(kw_args);
   Copy(kw_start, AvARRAY(kw_args), n_args, SV*);
   for (; kw_start <= kw_last; ++kw_start)
      *kw_start=PmEmptyArraySlot;
   AvFILLp(args)-=n_args;
   sv_magicext((SV*)args, kw_ref, PERL_MAGIC_ext, &stored_kw_vtbl, Nullch, 0);
   SvREFCNT_dec(kw_ref);  // compensate for increment in sv_magicext
}

void
fetch_stored_kw_args(args_ref)
   SV* args_ref;
PPCODE:
{
   MAGIC* mg=mg_findext(SvRV(args_ref), PERL_MAGIC_ext, &stored_kw_vtbl);
   if (mg != NULL) {
      PUSHs(mg->mg_obj);
   } else {
      PUSHs(&PL_sv_undef);
   }
}

void
bundle_repeated_args(args, first, end)
   AV* args=(AV*)SvRV(ST(0));
   I32 first;
   I32 end;
CODE:
{
   /* Equivalent to
        my @bundle;
        @bundle=splice @args, $first, $end-$first, \@bundle;
      but without touching REAL flags of the arrays and refcounts of the elements
   */
   AV* bundle=newAV();
   I32 n_args=AvFILLp(args)+1;
   I32 n_repeated=end-first;
   assert(n_repeated > 0 && end <= n_args);
   av_extend(bundle, n_repeated-1);
   if (!AvREAL(args)) AvREAL_off(bundle);
   Copy(AvARRAY(args)+first, AvARRAY(bundle), n_repeated, SV*);
   AvFILLp(bundle)=n_repeated-1;
   if (first+1 < end && end < n_args) Move(AvARRAY(args)+end, AvARRAY(args)+first+1, n_args-end, SV*);
   AvARRAY(args)[first]=newRV_noinc((SV*)bundle);
   AvFILLp(args)-=n_repeated-1;
}

void
unbundle_repeated_args(args, backtrack)
   AV* args=(AV*)SvRV(ST(0));
   AV* backtrack=(AV*)SvRV(ST(1));
CODE:
{
   // Undo bundle_repeated_args
   SV* pos_sv=av_pop(backtrack);
   I32 pos=SvIVX(pos_sv);
   SV* bundle_ref=AvARRAY(args)[pos];
   AV* bundle=(AV*)SvRV(bundle_ref);
   I32 n_repeated=AvFILLp(bundle)+1;
   I32 n_args=AvFILLp(args)+1;
   assert(SvTYPE(bundle)==SVt_PVAV && AvMAX(args) >= AvFILLp(args)+n_repeated-1);
   if (pos+1 < n_args) Move(AvARRAY(args)+pos+1, AvARRAY(args)+pos+n_repeated, n_args-pos-1, SV*);
   Copy(AvARRAY(bundle), AvARRAY(args)+pos, n_repeated, SV*);
   AvFILLp(args)+=n_repeated-1;
   AvREAL_off(bundle);
   SvREFCNT_dec(bundle_ref);
   SvREFCNT_dec(pos_sv);
}

void
learn_package_retrieval(objref, cvref)
   SV* objref;
   SV* cvref;
CODE:
{
   MAGIC* mg=sv_magicext(SvRV(objref), Nullsv, PERL_MAGIC_ext, &pkg_retrieval_index_vtbl, Nullch, 0);
   mg->mg_private=CvDEPTH(SvRV(cvref));
}

void
store_string_package_stash(hvref)
   SV* hvref;
CODE:
{
   string_stash=(HV*)SvRV(hvref);
}

void
store_integer_package_stash(hvref)
   SV* hvref;
CODE:
{
   integer_stash=(HV*)SvRV(hvref);
}

void
store_float_package_stash(hvref)
   SV* hvref;
CODE:
{
   float_stash=(HV*)SvRV(hvref);
}

BOOT:
   string_stash=gv_stashpv("Polymake::Overload::string", TRUE);
   integer_stash=gv_stashpv("Polymake::Overload::integer", TRUE);
   float_stash=gv_stashpv("Polymake::Overload::float", TRUE);
   UNIVERSAL_stash=gv_stashpv("UNIVERSAL", FALSE);
   if (PL_DBgv) {
      CvNODEBUG_on(get_cv("Polymake::Overload::can_signature", FALSE));
      CvNODEBUG_on(get_cv("Polymake::Overload::store_kw_args", FALSE));
      CvNODEBUG_on(get_cv("Polymake::Overload::fetch_stored_kw_args", FALSE));
      CvNODEBUG_on(get_cv("Polymake::Overload::bundle_repeated_args", FALSE));
      CvNODEBUG_on(get_cv("Polymake::Overload::unbundle_repeated_args", FALSE));
   }

=pod
// Local Variables:
// mode:C
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
=cut
