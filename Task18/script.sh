insmod main.ko
echo "Alice" > /dev/eudyptula
echo "Bob" > /dev/eudyptula
sleep 15
echo "Dave" > /dev/eudyptula
echo "Gena" > /dev/eudyptula
rmmod main
