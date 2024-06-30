# NOTE: This file is littered with learning notes. A lot of comments describe
# behavior/features inherent to the Perl programming language itself, which make
# them poor commenting practice in ordinary settings, but this is a learning
# exercise.

# Note that SCRIPTS end with .pl while MODULES end with .pm. This is like the
# distinction between drivers vs. libraries. Using .pm with modules, while not
# strictly necessary, is important because that ensures `use` statements in code
# that wants to import them can correctly resolve them.

# A class in Perl is a package. A package is essentially a namespace that
# separates the symbols (variables, subroutines, etc.) used inside it from those
# in other packages. ---ChatGPT
package Interpreter;

# Define compile-time constants. Note that to access them, you use them as if
# they were function calls e.g. `OPERATOR_RIGHT` instead of with a sigil
# `$OPERATOR_RIGHT`.
use constant {
    OPERATOR_RIGHT            => ">",
    OPERATOR_LEFT             => "<",
    OPERATOR_INCREMENT        => "+",
    OPERATOR_DECREMENT        => "-",
    OPERATOR_WRITE            => ".",
    OPERATOR_READ             => ",",
    OPERATOR_JUMP_IF_ZERO     => "[",
    OPERATOR_JUMP_UNLESS_ZERO => "]",

    CELL_MIN_VALUE => 0,
    CELL_MAX_VALUE => 255,
};

# The "constructor" of the class. Conventionally named "new", but it doesn't
# have to be. The main point is that it initializes and returns the class
# instance.
sub new {

    # The first argument supplied to new is the class itself. Note there are
    # multiple idiomatic ways to access arguments: unpacking e.g. `my ($arg1,
    # $arg2) = @_;` OR shifting like below, which you can think of like
    # "consuming" the arguments array.
    my $class       = shift;
    my $source      = ( shift or "" );
    my $memory_size = ( shift or 4096 );

    my @memory = (0) x $memory_size;

    # Attributes are saved as a hash reference, conventionally named $self. This
    # is what is treated as the object itself (the class instance). Note that we
    # use {} instead of () when initializing $self with a literal. This is a
    # shorthand for initializing $self to to be a hash REFERENCE instead of a
    # normal hash.
    my $self = {
        _source  => $source,
        _pc      => 0,
        _cells   => \@memory,    # Note the \: this is an array REFERENCE.
        _pointer => 0,

        # You can call subroutines before blessing, but they won't be able to
        # use $self. If they need $self, set undef here first and then call the
        # subroutine after blessing e.g. $self->_init_bracket_map().
        _bracket_map => _init_bracket_map($source),
    };

    # "Blessing" ties the new object to the class.
    bless $self, $class;
    return $self;
}

sub get_source {

    # Note that methods that want to access $self need to unpack it for
    # themselves from the arguments. The class instance is automatically passed
    # as the first argument to method calls.
    my ($self) = @_;

    # Because $self is a hash REFERENCE, you have to DEREFERENCE it with `->`
    # (like in C/C++) to key it. Also remember you key hashes with {key}, NOT
    # [key] like in other languages.
    return $self->{_source};
}

sub get_memory_size {
    my ($self) = @_;

    # Note the operations happening here: (1) Access the _cells member, which is
    # an array REFERENCE. (2) Dereference it with list context using @. (3)
    # Force scalar context with scalar() to get the number of array elements.
    return scalar @{ $self->{_cells} };
}

sub run {
    my ($self) = @_;

    # The length() function gets the number of characters in a string.
    my $num_instructions = length( $self->{_source} );

    while ( $self->{_pc} < $num_instructions ) {

        # Apparently getting a character from a string requires getting a
        # substring of length 1. There isn't a simple indexing operation like in
        # most other languages.
        my $operator = substr( $self->{_source}, $self->{_pc}, 1 );

        # Remember string comparisons use eq, ne, etc., NOT ==, !=, etc.!
        if ( $operator eq OPERATOR_RIGHT ) {
            $self->_move_pointer_right();
        }
        elsif ( $operator eq OPERATOR_LEFT ) {
            $self->_move_pointer_left();
        }
        elsif ( $operator eq OPERATOR_INCREMENT ) {
            $self->_increment_cell();
        }
        elsif ( $operator eq OPERATOR_DECREMENT ) {
            $self->_decrement_cell();
        }
        elsif ( $operator eq OPERATOR_WRITE ) {
            $self->_write_cell_to_stdout();
        }
        elsif ( $operator eq OPERATOR_READ ) {
            $self->_replace_cell_with_stdin();
        }
        elsif ( $operator eq OPERATOR_JUMP_IF_ZERO ) {
            $self->_jump_if_cell_is_zero();
        }
        elsif ( $operator eq OPERATOR_JUMP_UNLESS_ZERO ) {
            $self->_jump_if_cell_is_nonzero();
        }

        $self->{_pc}++;
    }
}

sub _init_bracket_map {
    my ($source) = @_;

    # Remember the sigils ($, @, %) are what define the types of the variables
    # and thus the context of the incoming initializers. Below, the empty list
    # () becomes an empty hash and empty array respectively.
    my %bracket_map   = ();
    my @bracket_stack = ();

    # Note that if I didn't need the index, an idiomatic way to iterate over a
    # string would be to split the string on nothing, turning it into an array
    # of characters: `for my $char ( split ( //, $string ) ) { ... }`.
    for my $address ( 0 .. length($source) - 1 ) {
        my $operator = substr( $source, $address, 1 );

        if ( $operator eq OPERATOR_JUMP_IF_ZERO ) {
            push( @bracket_stack, $address );

            # `continue` has a different use in Perl. `next` is Perl's
            # equivalent of `continue` in other languages. Perl also doesn't
            # have `break` but rather `last`.
            next;
        }

        if ( $operator eq OPERATOR_JUMP_UNLESS_ZERO ) {

            # pop() returns `undef` if the array is empty. It does not throw.
            my $opener_address = pop(@bracket_stack);

            # Extraneous closer. Note you can use the defined() function to
            # check if a value is not `undef`.
            if ( not defined $opener_address ) {

                # `die` does not exit with a custom code like `exit` can (`die`
                # usually uses 255). Since our tests check for an exit of 1 for
                # this case, we use `print STDERR` and `exit 1`. Otherwise,
                # `die` seems to be the preferred (and more concise) way to
                # print an error message and abort.
                print STDERR "Brainfuck syntax error: mismatched "
                  . OPERATOR_JUMP_UNLESS_ZERO . "\n";
                exit 1;
            }

            my $closer_address = $address;
            $bracket_map{$opener_address} = $closer_address;
            $bracket_map{$closer_address} = $opener_address;
        }
    }

    # Extraneous opener(s). Note that, similar in Python, you can test the array
    # directly to check if it's non-empty. This works because Perl also has a
    # concept of truthiness. The empty list () is false in a boolean context.
    if (@bracket_stack) {
        print STDERR "Brainfuck syntax error: mismatched "
          . OPERATOR_JUMP_IF_ZERO . "\n";
        exit 1;
    }

    return \%bracket_map;    # Note the \: this is returning a hash REFERENCE.
}

sub _move_pointer_right {
    my ($self) = @_;
    my $memory_size = $self->get_memory_size();
    $self->{_pointer} = ( $self->{_pointer} + 1 ) % $memory_size;
}

sub _move_pointer_left {
    my ($self) = @_;
    if ( $self->{_pointer} == 0 ) {
        $self->{_pointer} = $self->get_memory_size() - 1;
    }
    else {
        $self->{_pointer}--;
    }
}

sub _increment_cell {
    my ($self) = @_;
    my $pointer = $self->{_pointer};
    if ( $self->{_cells}[$pointer] == CELL_MAX_VALUE ) {
        $self->{_cells}[$pointer] = CELL_MIN_VALUE;
    }
    else {
        $self->{_cells}[$pointer]++;
    }
}

sub _decrement_cell {
    my ($self) = @_;
    my $pointer = $self->{_pointer};
    if ( $self->{_cells}[$pointer] == CELL_MIN_VALUE ) {
        $self->{_cells}[$pointer] = CELL_MAX_VALUE;
    }
    else {
        $self->{_cells}[$pointer]--;
    }
}

sub _write_cell_to_stdout {
    my ($self) = @_;
    my $cell_value = $self->{_cells}[ $self->{_pointer} ];

    # chr() gets the character string from ASCII value.
    print chr($cell_value);
}

sub _replace_cell_with_stdin {
    my ($self) = @_;

    my $input_byte;

    # Note the similarity to C system calls. We pass as an argument a variable
    # to initialize while the function return value is used for status checking.
    if ( read( STDIN, $input_byte, 1 ) == 1 ) {
        $self->{_cells}[ $self->{_pointer} ] = $input_byte;
    }
    else {
        die "Error reading from stdin: $!\n";
    }
}

sub _jump_if_cell_is_zero {
    my ($self) = @_;
    my $cell_value = $self->{_cells}[ $self->{_pointer} ];
    if ( $cell_value == 0 ) {
        my $destination_address = $self->{_bracket_map}{ $self->{_pc} };
        $self->{_pc} = $destination_address;
    }
}

sub _jump_if_cell_is_nonzero {
    my ($self) = @_;
    my $cell_value = $self->{_cells}[ $self->{_pointer} ];
    if ( $cell_value != 0 ) {
        my $destination_address = $self->{_bracket_map}{ $self->{_pc} };
        $self->{_pc} = $destination_address;
    }
}

# "Return true" to indicate that the module loaded successfully.
1;
