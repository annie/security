COMS 4187 Homework 2

To run my testing script, run `python test.py`.

There are several issues that I was unable to fix before submission time. Though I believe my sanitization method correctly parses the input (based on manual testing), when running my testing script I ran into some unpredictable behavior - for example, issues where the `decrypt` command was unable to find the file to decrypt (although it exists), and `BIO` errors when reading passwords from keyfiles. I was ultimately unable to debug these issues, but I thought I'd mention them here.

I also ran out of time to implement the functionalities to read from stdin and the mkdir command. I spent a great deal of time on the logic of the input sanitization function, and I hope that shows in my code.