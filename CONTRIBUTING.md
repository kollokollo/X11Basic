Guide to contributing to X11-Basic
==================================

X11-Basic exists for quite a while now. Therefore I consider it to be in a
quite mature state. I am planning not to extend the language much, e.g. I do
not want to add many more commands or functions. The language as it is should
stay much the same as it is now. 

However, there are still some bugs and also some of the features mentioned in 
the manual are not implemented. 

As there are:

1. The Array operators a(1:2,3) with a specified range are not implemented. 
   Always a complete row must be used: a(:,3) works fine.

2. Modifyable lvalued, like MIDS(t$,2,3)=a$ are not implemented,  also
   TIME$="12:04:06" does not work as you might expect (resulting in setting the
   systems time). However, an array with index range as a lvalue does already
   work.

These issues should be fixed somewhen. Also there are ideas for adding Bluetooth 
support and USB support to the Android version of X11-Basic. This should lead 
to as few additional commands and functions as possible. Maybe it can be done 
usinge external binaries called via the shell (like the SQL support is 
implemented).

Difficult bugs:
===============

There is a strange bug related to screen refresh in the Android version of 
X11-Basic. Does anybody have an idea what could be the problem?


More things left to do:
=======================
- test the thing and find more bugs,
- Optimize a bit more, improve performance,
- The WINDOWS-Version needs more work,
- work on the sound system,
- work on the SDL-Graphics implementation (fill styles etc...),
- documentation needs more work,
- port it to apple ipad/iphone,
(etc. etc.)

## License and attribution

All contributions must be properly licensed and attributed. If you are contributing your own original work, then you are offering it under a CC-BY license (Creative Commons Attribution). If it is code, you are offering it under the GPL-v2. You are responsible for adding your own name or pseudonym in the Acknowledgments file, as attribution for your contribution.

If you are sourcing a contribution from somewhere else, it must carry a compatible license. The project was initially released under the GNU public licence GPL-v2 which means that contributions must be licensed under open licenses such as MIT, CC0, CC-BY, etc. You need to indicate the original source and original license, by including a comment above your contribution. 


## Contributing with a Pull Request

The best way to contribute to this project is by making a pull request:

1. Login with your Github account or create one now
2. [Fork](https://github.com/kollokollo/X11Basic#fork-destination-box) the X11Basic repository. Work on your fork.
3. Create a new branch on which to make your change, e.g.
`git checkout -b my_code_contribution`, or make the change on the `new` branch.
4. Edit the file where you want to make a change or create a new file in the `contrib` directory if you're not sure where your contribution might fit.
5. Edit `doc/ACKNOWLEGEMENTS` and add your own name to the list of contributors under the section with the current year. Use your name, or a github ID, or a pseudonym.
6. Commit your change. Include a commit message describing the correction.
7. Submit a pull request against the X11Basic repository.



## Contributing with an Issue

If you find a mistake and you're not sure how to fix it, or you don't know how to do a pull request, then you can file an Issue. Filing an Issue will help us see the problem and fix it.

Create a [new Issue](https://github.com/kollokollo/X11Basic/issues/new) now!



## Thanks

We are very grateful for your support. With your help, this BASIC implementation will be a great project. 


So you are welcome to help.
