#!/usr/bin/perl -w

use strict;

####################################################################################################
# this perl script is used to convert vr9 chiptest/palladium image to support new ddr model in vr9 #
####################################################################################################

my $versioninfo = "V1.0.0 29 Oct 2008";

my $infile = "";
my $outfile = "";

my $read_line;
my $line1;
my $line2;
my $line3;
my $line4;

#
# Verify the input/output parameters 
#

if (($#ARGV + 1) != 2 ) {
    die "Usage: $0 <input asc file> <output asc file>\n"
}

$infile = shift(@ARGV);
$outfile = shift(@ARGV);

#
# Read/Write file handler
#
open (INFILE, $infile) || go_die("Can't open $infile fo read :$!", 1);

open (OUTFILE, ">$outfile") || go_die("Can't open $outfile for write :$!", 1);

#
# Force it to binary mode 
#
binmode (INFILE);

binmode (OUTFILE);

#
# to skip @00000000, so that it still inherits the original address
#
$read_line = <INFILE>;  
print OUTFILE $read_line;
while (1) {
    
    if (eof(INFILE)){
        go_die ("$0: Convert $infile to $outfile", 1)
    }
    $line1 = <INFILE>;
    if (eof(INFILE)){
        print OUTFILE $line1;
        go_die ("$0: Convert $infile to $outfile", 1)
    }    
    $line2 = <INFILE>;
    if (eof(INFILE)){
        print OUTFILE $line1;
        print OUTFILE $line2;        
        go_die ("$0: Convert $infile to $outfile", 1)
    }   
    $line3 = <INFILE>;
    
    if (eof(INFILE)){
        print OUTFILE $line3;
        print OUTFILE $line1;
        print OUTFILE $line2;        
        go_die ("$0: Convert $infile to $outfile", 1)
    }    
    $line4 = <INFILE>;    
    
    print OUTFILE $line3;
    print OUTFILE $line4;
    print OUTFILE $line1;
    print OUTFILE $line2;

}

#********************************************************************************************

sub go_die
{
    my($message, $supressformat) = @_;

    if ($message) {
        print "$message";
    }

    close (INFILE);
    close (OUTFILE);
    
    print " done\n";
    exit (0);
}



