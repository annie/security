import os
import sys
import subprocess

def test_addqueue():
    print "------------ addqueue tests ------------"

    print "trying to add a file that does not exist"
    print "./addqueue test.txt"
    os.system("./addqueue test.txt")
    print "now adding 3 existing files"
    print "./addqueue file.txt file2.txt file3.txt"
    os.system("./addqueue file.txt file2.txt file3.txt")

    print

def test_showqueue():
    print "------------ showqueue tests ------------"

    print "showing all the files in the queue"
    print "./showqueue"
    os.system("./showqueue")

    print

def test_rmqueue():
    print "------------ rmqueue tests ------------"

    stdout, stderr = subprocess.Popen(["./showqueue"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

    rmqueue = ["./rmqueue"]
    for line in stdout.split("\n"):
        line = line.split("\t")
        if len(line) > 1:
            rmqueue.append(line[3])

    print

    pid = os.fork()
    if pid == 0:
        try:
            print "switching to user 100"
            os.setuid(100)
            print "user 100 does not own files in the queue, rmqueue should fail"
            print " ".join(rmqueue)
            stdout, stderr = subprocess.Popen(rmqueue, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
            print stderr
        finally:
            os._exit(0)

    os.waitpid(pid, 0)

    print "switching back to root user"
    print "root user owns files in the queue, rmqueue should complete successfully"
    print " ".join(rmqueue)
    stdout, stderr = subprocess.Popen(rmqueue, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
    print stdout


def main():
    os.system("make")

    print
    print "------------- BEGIN TESTS --------------"
    print

    test_addqueue()
    test_showqueue()
    test_rmqueue()

main()
