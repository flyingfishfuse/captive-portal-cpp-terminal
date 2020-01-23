# captive-portal-cpp-terminal

(Abandoned maybe forever? Rust is the way to go for this I think)

captive portal based in c++ with an ncurses based command line functionality consisting of several options allowing for implementation of the Beef Framework and Bettercap in your attack... or even legal use, this is going to have an actual (hopefully)  secure authentication implementation using hashing and maybe other methods

I want to make this work on windows but that is definetely going to happen AFTER I finish way more of this


the following libraries are needed ta this point to compile:

  https://github.com/etr/libhttpserver
  Boost/algorithm 
  Boost/filesystem
  Boost/program_options 
  libmicrohttpd
  
  The idea of this tool is that it presents a captive portal setup to the network of your choice. You can either choose to make this YOUR network or SOMEONE ELSE'S (illegal without permission) If you choose to use someone elses network,  I give you a few choices how. I also give you the option to beefhook every host that gets caught in the attack
  
  TODOTOFU:
  You aren't zeroing out entered passwords as far as I can tell, those could end up latent on the stack/heap. (now that that guy said that, this means I officially want to make my own memory allocation function specifically for the passwords to abosfuckinglutley g 
  You're passing an arbitrary string in to system in establish_MITM, thats very dangerous (that code was originally part of a hacking tool I made and has not been "safed" yet, I literally copy pasted it from my snippets. It was never meant to be secure it was meant to be quick to edit and run on a dual Kali VM setup that would do wierd things.)
