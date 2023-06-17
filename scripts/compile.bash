echo Compiling...

cd ../src
pwd | find ~+ -name '*.cpp' > ../source.txt
cd ..

sed -i 's/\.\///g' source.txt
sed -i 's/ /\ /g' source.txt

INCLUDE="-Isrc -Ilib/SFML/include"
SFML="-lsfml-graphics -lsfml-window -lsfml-network -lsfml-system"
FLAGS="-Wall -Werror -Wpedantic"

if [ $1=="debug" ]; then
    DEBUG=-g
fi

g++ -std=c++20 @source.txt $INCLUDE $LINK $SFML $FLAGS -o bin/gameoflife.exe

RETURN=$?

if [ $RETURN == 0 ]; then
    echo Success!
else
    echo Failed!
fi

exit $RETURN
