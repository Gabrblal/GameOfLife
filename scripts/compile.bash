echo Compiling...

cd ..

if [ $1=="debug" ]; then
    make debug
elif [ $1=="release" ]; then
    make gameoflife
fi

RETURN=$?

if [ $RETURN == 0 ]; then
    echo Success!
else
    echo Failed!
fi

exit $RETURN
