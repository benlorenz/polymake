# input for generate_ninja_targets.pl

( 'Main.cc' => join(" ", map { "-DPOLYMAKE_CONF_$_='\${$_}'" } grep { defined $ConfigFlags{$_} } qw(InstallTop InstallArch Arch FinkBase BundledExts)),
)
