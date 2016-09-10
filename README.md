# FSAE Model Year 2016

Welcome to the FSAE MY16 codebase! There is actually no real code in this specific repository; rather, this contains the structure of files and libraries necessary to conform to the [Bare Arduino Project](https://github.com/ladislas/Bare-Arduino-Project) specification. This framework allows nice things like upload using `make` and having live code compilation on your editor of choice, and I highly recommend it for any non-trivial Arduino projects you might make in the future.

Follow the instructions below to start devving! For newbies, I recommend using the Arduino IDE for now since it is easy to use and you don't need to mess with the command line; as you become more proficient and develop a favorite text editor, I recommend reading about how to use `make` and `make upload` to compile and run your code without needing to be in the Arduino editor. 

Arduino-IDE instructions are given first, and instructions for the more advanced users are below. Please create an [issue](https://guides.github.com/features/issues/) for this repository if any part of this documentation is incorrect, or even better make a [pull request](https://help.github.com/articles/about-pull-requests/) to fix it!

Basic Instructions:

1. Create a folder for this project in a memorable place.

2. Install [git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git) if you haven't already. Git is a version control system that makes it easy for multiple people to collaborate on a large codebase. Think Dropbox shared folders or the MechE's EPDM, but 100x better and allowing for things like merging simultaneous changes and viewing different branches. If you're Course 6 at MIT, you will definitely use this in many of your classes so it's worth learning now!

3. Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software). The version shouldn't matter, so just use the most recent one, but let me know if there is a problem with it!

4. [Clone this repository](https://help.github.com/articles/cloning-a-repository/) to the folder you created in step 1.

5. Open the `/src` folder from the cloned repository, clone any of the `MY16_*` projects that you care about from the [MITMotorsports organization](https://github.com/MITMotorsports), and open their associated `.ino` files in the Arduino IDE. You are now coding!
