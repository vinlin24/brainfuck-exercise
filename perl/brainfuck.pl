#!/usr/bin/perl
# NOTE: This file is littered with learning notes. A lot of comments describe
# behavior/features inherent to the Perl programming language itself, which make
# them poor commenting practice in ordinary settings, but this is a learning
# exercise.

# Best practice to include these pragmas to enforce stricter codding practices.
use strict;
use warnings;

# Include the subdirectory (within our project directory) in the module search
# path. Note that "perl" here refers to what we named the subdirectory, not to
# be confused as anything special related to the Perl runtime itself.
use lib "perl";

# Import the class (package) we write in another module.
use Interpreter;

sub main {

    # An array in scalar context turns into the number of elements in the array.
    # Remember that the sigils ($, @, %) define the type of the variable and
    # thus the context of its incoming initializers.
    my $argc = @ARGV;

    # In Perl, the first command line argument is at ARGV[0], not ARGV[1].
    # What's normally argv[0] in other languages is instead stored in $0. Also
    # note that `test or die ...` is a common idiom.
    $argc > 0 or die( get_usage() . "\n" );

    my $source_path = $ARGV[0];
    my $source      = load_source_file($source_path);

    # Instantiate our custom Perl class and invoke a method on it.
    my $interpreter = new Interpreter($source);
    $interpreter->run();
}

sub get_usage {
    return "usage: $0 FILE";
}

sub load_source_file {
    my ($source_path) = @_;

    # `operation or die ...` is a common idiom. For `die` messages for system
    # calls, it's useful to include $!. $! holds the OS error message from the
    # last system call that failed.
    open( my $source_fh, "<", $source_path )
      or die "Cannot open $source_path for reading: $!\n";

    # I/O: a file handle `<fh>` in scalar context automatically reads the entire
    # buffer into the scalar string.
    my $source = <$source_fh>;

    close($source_fh);

    return $source;
}

main();
