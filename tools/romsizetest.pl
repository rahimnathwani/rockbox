#!/usr/bin/perl

#
# Check that the given file is smaller than the given size and if not, return
# an error code. Used to verify that the rombox.ucl file fits on the particular
# model you build for.

sub filesize {
    my ($filename)=@_;
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
        $atime,$mtime,$ctime,$blksize,$blocks)
        = stat($filename);
    return $size;
}

my $max = $ARGV[0];
my $file = filesize($ARGV[1]);

if($file > $max ) {
    print "Output is larger than max ($max)\n";
    exit 1;
}
