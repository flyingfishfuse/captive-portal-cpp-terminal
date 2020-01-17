# captive-portal-cpp-terminal
captive portal based in c++ with an ncurses based command line functionality consisting of several options allowing for implementation of the Beef Framework and Bettercap in your attack... or even legal use, this is going to have an actual (hopefully)  secure authentication implementation using hashing and maybe other methods


the following libraries are needed ta this point to compile:

  https://github.com/etr/libhttpserver
  Boost/algorithm 
  Boost/filesystem
  Boost/program_options 
  
  I'm NOT going ot be done with this for a long time but I'm going to make this my primary project... it has enough different parts I can jump around to keep from burning out on any one single part. Its also neato and useful and I haven't seen what I'm trying to do except for fluxion and its older and doesn't do what Im planning.
  
  The idea of this tool is that it presents a captive portal setup to the network of your choice. You can either choose to make this YOUR network or SOMEONE ELSE'S (illegal without permission) 
  
  If you choose to use someone elses network,  I give you a few choices how.
 
  I also give you the option to beefhook every host that gets caught in the attack
  
  TODOTOFU:
  You aren't zeroing out entered passwords as far as I can tell, those could end up latent on the stack/heap.
