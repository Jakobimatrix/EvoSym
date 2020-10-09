if [ -e src/executables/evosym_start ]; then
    exec src/executables/evosym_start
else
    echo "Cannot find executable."
fi
