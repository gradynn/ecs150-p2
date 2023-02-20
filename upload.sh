test_name='sem_simple' 

echo "Enter CSIF username: "
read username
ssh $username@pc02.cs.ucdavis.edu "rm -rf ecs150-p2"
echo "Remnants of previous upload removed"
scp -rq ../ecs150-p2 $username@pc02.cs.ucdavis.edu:~/ecs150-p2
echo "New upload complete"
ssh $username@pc02.cs.ucdavis.edu "cd ./ecs150-p2/apps ; make clean ; cd ../libuthread ; make ; cd ../apps ; make $test_name.x ; ./$test_name.x" 