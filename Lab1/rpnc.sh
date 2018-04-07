#
# Bash Reverse Polish Notation (RPN) Calculator
#
# Accepts an arbitrary number of arguments in RPN, evaluates them and prints the answer to stdout.
#
# Inspiration source: David Harrison, UCSC-CMPS109, 4/5/18, ~12:45pm.

stack=()
next=0

rpn=`echo "$1" | sed 's/\*/x/g'`

for token in $rpn
do
  if [[ "$token" =~ ^[0-9]+$ ]]
  then
    stack[$next]=$token
    (( next += 1 ))
  else
    (( left_index = next - 2 ))
    (( right_index = next - 1 ))
    (( next -= 2))
    left_oper=${stack[left_index]}
    right_oper=${stack[right_index]}
    if [[ "$token" == "+" ]]
    then (( newVal = left_oper + right_oper ))
    elif [[ "$token" == "-" ]]
    then (( newVal = left_oper - right_oper ))
    elif [[ "$token" == "/" ]]
    then (( newVal = left_oper / right_oper ))
    elif [[ "$token" == "^" ]]
    then (( newVal = left_oper ** right_oper ))
    elif [[ "$token" == "x" ]]
    then (( newVal = left_oper * right_oper ))
    fi
    stack[$next]=$newVal
    (( next += 1 ))
  fi
done
echo ${stack[0]}
