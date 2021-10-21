# Report

By **Elina Houde** and **Nicolas Vincent**.

## Exercise 1 : Optimize

### Q1

The first compilation produces a binary that has the **expected behavior** iee when the sum overflows the error is detected and the value is wrapped around (which is the behavior by default).

The second compilation produces a binary that **doesn't detect the overflow error** because the `-O2` optimisation assumes that `a+b` is always positive and therefore it deletes the if statement.

The third compilation produces a binary that has the expected behavior since the option `-fno-strict-overflow` tells the compiler that an overflow might happen and that it shouldn't do any optimisation concerning signed integers' arithmetics.

The fourth compilation produces a binary that also has the expected bahvior because `-fno-strict-overflow` prevents the `-O2` optimisation from doing arithmetic optimisation.

### Q2

Replace lines 11-14 by :
```C
if (a > INT_MAX - b) {
  printf("overflow error \n");
  return (-1);
}
```

## Exercise 2 : WinLoose

### Q2

**Print "You win!" or infinite loop**
```bash
$ ./winloose 1 8
address of x: 0x7ffe19c954db
address of t1: 0x7ffe19c954d3
0 : 0x7ffe19c954d3: 1
1 : 0x7ffe19c954d4: 1
2 : 0x7ffe19c954d5: 1
3 : 0x7ffe19c954d6: 1
4 : 0x7ffe19c954d7: 1
5 : 0x7ffe19c954d8: 1
6 : 0x7ffe19c954d9: 1
7 : 0x7ffe19c954da: 1
8 : 0x7ffe19c954db: 1  # address of x
value of x : 0x7ffe19c954db: 1
You win !
```
We win because the variable `x` was overwritten by the last iteration of the loop. When the second parameter is 8 (the size of the array + 1), we are sure to win because we overwrite only x.

```bash
address of x: 0x7fffca78066b
address of t1: 0x7fffca780663
Before modification - 0 : 0x7fffca780663: -54
0 : 0x7fffca780663: 9
Before modification - 1 : 0x7fffca780664: -1
1 : 0x7fffca780664: 9
Before modification - 2 : 0x7fffca780665: 127
2 : 0x7fffca780665: 9
Before modification - 3 : 0x7fffca780666: 0
3 : 0x7fffca780666: 9
Before modification - 4 : 0x7fffca780667: 0
4 : 0x7fffca780667: 9
Before modification - 5 : 0x7fffca780668: 0
5 : 0x7fffca780668: 9
Before modification - 6 : 0x7fffca780669: 0
6 : 0x7fffca780669: 9
Before modification - 7 : 0x7fffca78066a: 0
7 : 0x7fffca78066a: 9
Before modification - 8 : 0x7fffca78066b: 0
8 : 0x7fffca78066b: 9
Before modification - 9 : 0x7fffca78066c: 9  # i value is overwritten :
9 : 0x7fffca78066c: 9                        # old value=9 and new value=9
Before modification - 10 : 0x7fffca78066d: 0
2314 : 0x7fffca780f6d: 0                      # i value is overwritten again
Before modification - 2315 : 0x7fffca780f6e: 0
2315 : 0x7fffca780f6e: 9
Before modification - 2316 : 0x7fffca780f6f: 0
2316 : 0x7fffca780f6f: 9
Before modification - 2317 : 0x7fffca780f70: 0
2317 : 0x7fffca780f70: 9
Before modification - 2318 : 0x7fffca780f71: 0
2318 : 0x7fffca780f71: 9
...
Before modification - 7166 : 0x7fffca782261: 57
7166 : 0x7fffca782261: 9
Before modification - 7167 : 0x7fffca782262: 0
7167 : 0x7fffca782262: 0
Before modification - 7168 : 0x7fffca782263: 50
7168 : 0x7fffca782263: 0                         # i value is overwritten again => break the loop
0x7fffca78066b: 9
You win !
```
If the second parameter is greater than 8, we can overwrite more addresses. Sometimes we win. Otherwise we get `Erreur de segmentation`.


**Print "You loose"**

We loose when the variable `x` wasn't overwritten ie. the second parameter is less than the size of the array (`8`).


**Infinite loop**

We can have an infinite loop when the second parameter is greater than 8 (the value of `i` is overwritten) and less than the second parameter. For instance, with the arguments 1 9 :

```bash
$ ./winloose 1 9
address of x: 0x7ffe510b05ab
address of t1: 0x7ffe510b05a3
Before modification - 0 : 0x7ffe510b05a3: 81
0 : 0x7ffe510b05a3: 8
Before modification - 1 : 0x7ffe510b05a4: -2
1 : 0x7ffe510b05a4: 8
Before modification - 2 : 0x7ffe510b05a5: 127
2 : 0x7ffe510b05a5: 8
Before modification - 3 : 0x7ffe510b05a6: 0
3 : 0x7ffe510b05a6: 8
Before modification - 4 : 0x7ffe510b05a7: 0
4 : 0x7ffe510b05a7: 8
Before modification - 5 : 0x7ffe510b05a8: 0
5 : 0x7ffe510b05a8: 8
Before modification - 6 : 0x7ffe510b05a9: 0
6 : 0x7ffe510b05a9: 8
Before modification - 7 : 0x7ffe510b05aa: 0
7 : 0x7ffe510b05aa: 8
Before modification - 8 : 0x7ffe510b05ab: 0
8 : 0x7ffe510b05ab: 8
Before modification - 9 : 0x7ffe510b05ac: 9  # i value 
8 : 0x7ffe510b05ab: 8                        # is overwritten 
Before modification - 9 : 0x7ffe510b05ac: 9
8 : 0x7ffe510b05ab: 8
```

We have the following execution stack :

| 3  | 2          | 1 | 0 |(bytes) |
| :---------------: |:---------------:| :-----:| :-----:| :----:|
| t[3]  |   t[2]   |    t[1]   |    t[0]  | -> address 0x7ffe510b05a3 to 0x7ffe510b05a6 |
|   t[7]  |   t[6]   |    t[5]   |    t[4]  | -> address 0x7ffe510b05a7 to 0x7ffe510b05aa |
|         |          |     i     |     x    | -> address 0x7ffe510b05ab to 0x7ffe510b05ae |

x is stored after the array t. Then i is stored.

So, when the program arrives to the last turns of the loop (i=8), we overwrite t[8] which corresponds to x. Then i=9, we overwrite t[9], which corresponds to i. So, the value of i, which is 9, becomes 8. Then, we executes again the same operation for i=9, then i=8, i=9...

### Q3

We can try with `1 8`, `9 11` or `2 8`. With any number not nul and `8` we win.

## Q4

We can use `ghidra`:

```bash
                             **************************************************************
                             *                          FUNCTION                          *
                             **************************************************************
                             undefined main()
             undefined         AL:1           <RETURN>
             undefined4        Stack[-0xc]:4  local_c                                 XREF[7]:     00101168(W), 
                                                                                                   00101186(R), 
                                                                                                   0010118f(R), 
                                                                                                   001011a0(R), 
                                                                                                   001011a9(R), 
                                                                                                   001011c4(RW), 
                                                                                                   001011db(R)  
             undefined1        Stack[-0xd]:1  local_d                                 XREF[2]:     00101164(W), 
                                                                                                   001011e0(R)  
             undefined1        Stack[-0x15]:1 local_15                                XREF[1]:     0010119c(*)  
             undefined4        Stack[-0x1c]:4 local_1c                                XREF[1]:     0010115d(W)  
             undefined8        Stack[-0x28]:8 local_28                                XREF[3]:     00101160(W), 
                                                                                                   00101171(R), 
                                                                                                   001011c8(R)  
                             main                                            XREF[4]:     Entry Point(*), 
                                                                                          _start:0010108d(*), 00102050, 
                                                                                          001020f8(*)  
        00101155 55              PUSH       RBP
```

 `local_c` is `i` at address `Stack[-0xc]`. 

 `local_d` is `x` at address `Stack[-0xd]`. 
 
 `local_15` is `t1` at address `Stack[-0x15]`.

### Q5

```bash
$ ./winloose-protected 1 8
0 : 0x7fff55cd2640: 1
1 : 0x7fff55cd2641: 1
2 : 0x7fff55cd2642: 1
3 : 0x7fff55cd2643: 1
4 : 0x7fff55cd2644: 1
5 : 0x7fff55cd2645: 1
6 : 0x7fff55cd2646: 1
7 : 0x7fff55cd2647: 1
8 : 0x7fff55cd2648: 1
You loose ...
*** stack smashing detected ***: terminated
Abandon
```

The stack smashing is detected.

```bash
./winloose-protected 9 23342346
0 : 0x7ffe3056f600: 9
1 : 0x7ffe3056f601: 9
2 : 0x7ffe3056f602: 9
3 : 0x7ffe3056f603: 9
4 : 0x7ffe3056f604: 9
5 : 0x7ffe3056f605: 9
6 : 0x7ffe3056f606: 9
7 : 0x7ffe3056f607: 9
8 : 0x7ffe3056f608: 9
9 : 0x7ffe3056f609: 9
10 : 0x7ffe3056f60a: 9
11 : 0x7ffe3056f60b: 9
...
270 : 0x7ffe3056f70e: 9
271 : 0x7ffe3056f70f: 9
272 : 0x7ffe3056f710: 9
273 : 0x7ffe3056f711: 0
274 : 0x7ffe3056f712: 0
275 : 0x7ffe3056f713: 0
Erreur de segmentation
```

The behaviour is undefined, we now have a segfault.

### Q6

There is a new label : `__stack_chk_fail` in the `objdump` output.

```C
if (local_10 == *(long *)(in_FS_OFFSET + 0x28)) {
  return 0;
}
                /* WARNING: Subroutine does not return */
__stack_chk_fail();
```
We can see in `ghidra` the canary use at the end of the function.

## Exercise 3 : isPasswordOK?

### Q1

Possible behaviours for `gets` :
- On success, the function returns `str`.
- If the end-of-file is encountered while attempting to read a character, the `eof` indicator is set (`feof`). If this happens before any characters could be read, the pointer returned is a null pointer (and the contents of `str` remain unchanged).
- If a read error occurs, the error indicator (`ferror`) is set and a null pointer is also returned (but the contents pointed by `str` may have changed).

### Q2

**Attack to crash the execution**

Enter a long string (for instance : `"*ùmlkjtrezqwxcvhjiolùmjytrdszedfguiklm$=àç_ytrezaqsdfghjklmlnbvcxwqazertyuioplkjhgfdcvbn"`)

When the user types a long string, gets will copy all the string in the variable `Password` without verifying if the entry is shorter than the size of the variable. So, gets write the first characters of the long string in `Password`, and the others characters in the next memory : it will rewrite a part of the stack and, if the string is enough long, rewrite the return address of the function and will raise an error at the compilation (`"Erreur de segmentation (core dumped)"`).

---

**Attack to get authenticated without entering twice the correct password**

This attack is in two steps : 
- first time : enter the good password,
- second time : CRTL-D.

The first time, we enter the password, the password is stored in the variable `Password` created in the function `IsPasswordOK()`. The second time, when we call the function `IsPasswordOK()`, the variable `Password` is allocated at the same address than previously. As there are any instanciation of the variable, its default value is the previous one, i.e. the previous password written by the user. Then, we call the function `gets()` and send to this function `"CTRL-D"`, which corresponds to the end-of-file character. As see in the first question, when the function `fgets` encounter the end-of-file before any characters coulds be read, the contents of the variable `Password` remain unchanged. Then, when we compare `Password` with the good password, we compare in reality the good password and the password which the user types the first time and which are always stored in `Password`. So, the comparaison succeed.

### Q3
Possible behaviours for `fgets` :
- On success, the function returns `str`.
- If the end-of-file is encountered while attempting to read a character, the `eof` indicator is set (`feof`). If this happens before any characters could be read, the pointer returned is a null pointer (and the contents of `str` remain unchanged).
- If a read error occurs, the error indicator (`ferror`) is set and a null pointer is also returned (but the contents pointed by `str` may have changed).

It is the same behaviour than `gets` for return value.

Notice that `fgets` is quite different from `gets`: not only `fgets` accepts a stream argument, but also allows to specify the maximum size of `str` and includes in the string any ending newline character.

To replace `gets` by `fgets` in the code, we need to remove the ending newline character at the end of the password if it exists.

```C
unsigned int size=12;
int IsPasswordOK(void)
{
	char Password[size];
	//gets(Password);
	fgets(Password, size, stdin);
	if(Password[strlen(Password) - 1] == '\n') {
		Password[strlen(Password) - 1] = 0;
	}
	return 0 == strcmp(Password, "goodpass");
}
```

As previously, we can get authenticated without entering twice the correct password. To avoid this, we need to verify than `fgets` don't encounter the end-of-file before any characters could be read. For this, we must verify the value of the returned pointer :

```C
unsigned int size=12;
int IsPasswordOK(void)
{
	char Password[size];
	// Verification than the returned pointer is not NULL
	if (fgets(Password, size, stdin) == NULL) {
		return 0;
	}
	if(Password[strlen(Password) - 1] == '\n') {
		Password[strlen(Password) - 1] = 0;
	}
	return 0 == strcmp(Password, "goodpass");
}
```

The second attack (to crash the execution) is not possible anymore : `fgets` only copy at most the number of characters precised in parameter, that avoid stack overflow.

## Explanation about the rule O4. Integers (INT) - Subrule INT30-C

**Problem:**

When operations are computed on unsigned integers, if the resulting value is greater than the max size of an unsigned integers, the resulting value cannot be represented by an unsigned integer. In this case, this value is reduced modulo the max size of the unsigned integer type. This behavior is called unsigned integer wrapping and can result to an unexpected value which can lead to  an exploitable vulnerability. For instance, if the resulting value may be used to allocate memory, the program can allocate insufficient memory for a subsequent operation.

**Solution for the Addition (Precondition Test):**
```C
void func(unsigned int ui_a, unsigned int ui_b) {
  unsigned int usum;
  if (UINT_MAX - ui_a < ui_b) {
    /* Handle error */
  } else {
    usum = ui_a + ui_b;
  }
  /* ... */
}
```
We can test the operand before the operation to guarantee there is no possibility of unsigned wrap: for this, we can check that, by removing the first operand from the max value of the unsigned integer type, the result of this substraction is greater than the second operand.

In fact, we have a wrap if ui_a + ui_b > UINT_MAX i.e. ui_b > UINT_MAX - ui_a.


**Solution for the Addition (Postcondition Test):**
```C
void func(unsigned int ui_a, unsigned int ui_b) {
  unsigned int usum = ui_a + ui_b;
  if (usum < ui_a) {
    /* Handle error */
  }
  /* ... */
}
```

Otherwise, we can check after the computation the result of the operation to ensure there was no wrap. As we compute addition on two unsigned integers, the result may to be greater than each operand. So, we just need to check this property.


## Links

- [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)
- [INT32-C. Ensure that operations on signed integers do not result in overflow](https://wiki.sei.cmu.edu/confluence/display/c/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow)
- [MEM30-C. Do not access freed memory](https://wiki.sei.cmu.edu/confluence/display/c/MEM30-C.+Do+not+access+freed+memory)
- [ARR30-C. Do not form or use out-of-bounds pointers or array subscripts : Apparently accessible out-of-range index](https://wiki.sei.cmu.edu/confluence/display/c/ARR30-C.+Do+not+form+or+use+out-of-bounds+pointers+or+array+subscripts#ARR30C.Donotformoruseoutofboundspointersorarraysubscripts-ApparentlyAccessibleOut-of-RangeIndex)
- [ERR33-C. Detect and handle standard library errors : on mallocs](https://wiki.sei.cmu.edu/confluence/display/c/ERR33-C.+Detect+and+handle+standard+library+errors)
- [INT30-C. Ensure that unsigned integer operations do not wrap](https://wiki.sei.cmu.edu/confluence/display/c/INT30-C.+Ensure+that+unsigned+integer+operations+do+not+wrap)