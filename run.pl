#!/usr/bin/perl

#names of the input files
@canfiles = ("lab5a", "lab5b", 
             "lab7a", "lab7b", "lab7c", "lab7d", "lab7e", "lab7f", 
             "lab8a", "lab8b", "lab8c", "lab8d", "lab8e", "lab8f", 
             "lab9a", "lab9b", "lab9c", "lab9d", "lab9e", "lab9f", "lab9g",
             "lab10a", "lab10b", "lab10c", "lab10d", "lab10e",
             "lab10f", "lab10g", "lab10h", "lab10i", "lab10j",
             "lab10k", "lab10l", "lab10m", "lab10n", "lab10o",
             "lab11a", "lab11b", "lab11c", "lab11d", "lab11e",
             "lab11f", "lab11g", "lab11h", "lab11i", "lab11j",
             "lab12a", "lab12b", "lab12c", "lab12d", "lab12e",
             "lab12f", "lab12g", "lab12h", 
             "error1", "error2", "error3", "error4", "error5", "error6",
             "error7", "error8", "error9");


#subdirectory in which the output will be found
$dir = "Tests/";
$canDir = "/u/css/classes/3481/134/lab12/";

if (! -e $dir)
{
   print "need to create a Tests directory first\n";
   exit();
}

if (! -e "yess")
{
   print "missing yess executable\n";
   exit();
}

$pass = 0;

for ($i = 0; $i <= $#canfiles; $i++){
   $input = $canDir.$canfiles[$i].".yo";
   $canoutput = $canDir.$canfiles[$i].".dump";
   $studentoutput = "Tests/".$canfiles[$i].".dump";
   $studentproblems = "Tests/".$canfiles[$i].".problems";
   if (-e $input){
      print "Testing $canfiles[$i]. ";
      #remove the old ourdump and problems files
      system "rm -f $studentoutput $studentproblems";

      #run student yess on the input file
      #print "Creating $studentoutput \n";
      system "./yess $input > $studentoutput";
   
      #compare student ourdump file to teacher dump file
      #print "Comparing $canoutput and $studentoutput\n";
      system "diff -b -w $canoutput $studentoutput > $studentproblems";
   
      #if there are differences, keep the problem and ourdump files
      if (! system "test -s $studentproblems"){
         print "Failed.\n";
      } else {
         print "Passed.\n";
         system "rm -rf $studentoutput $studentproblems";
         $pass = $pass + 1;
      }
   } else {
      print "missing $input\n";
   }
}

$total = $#canfiles + 1;
print "$pass out of $total passed.\n";
if ($pass != $total) {
   print "See Tests directory for failed tests\n";
}


