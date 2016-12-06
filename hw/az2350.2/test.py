import os
import sys
import subprocess

def main():
    os.system("make")

    print
    print "------------- BEGIN TESTS --------------"
    print

    print "------------- BASIC COMMANDS --------------"
    os.system("mkdir basic")
    f_secret = open("basic/secret.txt", "w+")
    f_secret.write("top secret information\n")
    f_secret.close()

    f_basic = open("basic.txt", "w+")
    f_basic.write("cd basic\n")
    f_basic.write("keyfile key.txt\n")
    f_basic.write("password pass key.txt\n")
    f_basic.write("encrypt \"secret.txt\" encrypt.txt\n")
    f_basic.write("decrypt 'encrypt.txt' \"decrypted.txt\"\n")
    f_basic.close()

    os.system("./encdec basic.txt")

    print "------------- VALID INPUTS --------------"
    os.system("mkdir valid")
    f_sec_valid = open("valid/secret.txt", "w+")
    f_sec_valid.write("more secret information")
    f_sec_valid.close()

    f_valid = open("valid_inputs.txt", "w+")
    f_valid.write("cd valid\n")
    f_valid.write("keyfile key.txt\n")
    f_valid.write("password secret key.txt\n")
    f_valid.write("encrypt secret.txt encrypted.txt\n")
    f_valid.write("decrypt 'encrypted.txt' \"decrypted.txt\"\n")
    f_valid.write("encrypt secret.txt e.'ncry\"pted.t+xt\n")
    f_valid.write("decrypt 'e.\\'ncry\"pted.t+xt' decr\\ypted.tx\"t\n")
    f_valid.write("encrypt \"decrypted.txt\" \"re'encrypted.txt\"\n")
    f_valid.write("decrypt re'encrypted.txt 'de\\\crypted.txt'\n")
    f_valid.close()

    os.system("./encdec valid_inputs.txt")

    print "------------- INVALID INPUTS --------------"

    os.system("mkdir invalid")
    f_sec_invalid = open("invalid/secret.txt", "w+")
    f_sec_invalid.write("final secret information")
    f_sec_invalid.close()

    f_invalid = open("invalid_inputs.txt", "w+")
    f_invalid.write("cd invalid\n")
    f_invalid.write("keyfile key.txt\n")
    f_invalid.write("password passcode key.txt\n")
    f_invalid.write("encrypt \"abc encrypted.txt\n")
    f_invalid.write("decrypt 'encrypted.txt' pqr stuv\n")
    f_invalid.write("encrypt secret.txt 'jklm\"\n")
    f_invalid.write("decrypt \"enc\\\\\\rypted\" 'decr\\ypted.tx\"t\n")
    f_invalid.close()

    os.system("./encdec invalid_inputs.txt")

main()