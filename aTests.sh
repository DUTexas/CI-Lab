#aTests.sh

for TESTFILE in test_binary.txt test_format.txt test_simple.txt test_string.txt test_ternary.txt test_unary.txt test_variable.txt
do
echo "____$TESTFILE ____"
./ci -i ./tests/$TESTFILE 
./ci_reference -i ./tests/$TESTFILE 
done