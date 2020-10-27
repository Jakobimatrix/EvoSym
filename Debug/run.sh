exe="src/executable/evosym_start"
if [ -e $exe ]; then
    exec $exe
else
    echo "Cannot find executable."
fi
