# simpletrace
A very basic ray tracer which works on raspberry pi

![Example output](https://github.com/daddycoder0/simpletrace/blob/master/sample.jpg "Example output")

## History

A few nights ago, I was scrolling through reddit and came across the latest cool raspberry pi project someone had done, and it got me to thinking. I had been doing some fiddling myself at the start of 2015 before (luckily for me), I became a father in the May. As a result, I get now *very* little free time.

My initial motivation with this was just to create *something* that worked on raspberry pi (just because), and a simple ray tracer seemed nice and straight forward. I did however get a lot of satisfaction knowing that the little single card computer hanging out the back of the TV in my lounge was busy firing rays and drawing the results :)

Subsequently, I have all sorts of ideas I'd like to try on this project (or similar ones), but very little time to try them. My (perhaps unrealistic?) hope was that by posting it on here, someone maybe be able to take something from the code, or just be inspired to do something similar.

## Features

As it stands the project is very simple, a brief overview of current functionality:

* Supports a very basic scene file in an xml format
* Supports super sampling
* Basic shadow casters
* Fairly cross platform (as far as I know)
* Writes out bitmap results
* Will try to run multiple threads where appropriate (should mean that running on a pi 3 for example is much quicker than pi 1, but this is untested)

## How to use

My objective was to get something running on the pi, but I didn't fancy doing much development on the device itself, which is why I used [cmake](https://cmake.org/) to give me a means of easily compiling for the pi as well as creating a VS workspace on my laptop. Assuming cmake is installed on the pi, running that followed by make should spit out a working executable.

To render the scene, the command line looks something like:

`./simpletrace <scene xml> <width> <height> <super sampling>`

As my pi is plugged into the back of my lounge TV (which is nearly always in use), I relied on various tactics to test the program. My actual source was stored in an SVN repository hosted on the pi itself, so once I'd finished fiddling on the laptop, I would commit the code, and then SSH into the pi to update and build the code and run some tests. In order to see the image back I use tricks like:

`python -m SimpleHTTPServer 8000`

to allow me to pull the image remotely through a browser. This all meant that I could secretly carry on fiddling with my nerdy stuff using my phone long after the wife had told me to put the laptop away ;)

## Known issues

Its been nearly two years since I did any work on this, but I've spotted a few issues:

* There's absolutely no sanity checking on the command line args, so if you pass in garbage it just dies.

I'm sure there's plenty more problems! :)

## Ideas


As mentioned above, I was making most progress by working in Windows and then compiling/running on the pi later via terminal. Something I have done previously when having to frequently re-run the same commands on similar systems, is write a basic web server (I would normally use [cherrypy](http://cherrypy.org/)) which will execute the commands on request via a basic web interface (and pipe the console log back to the page). If we're getting really fancy, the same server could read in the scene file itself, and then put a realtime view of the scene up in the browser using something like [threejs](https://threejs.org/)! 

UPDATE: Basic realtime preview, which reads scene file direct from github [here](https://daddycoder0.github.io/hub). Was hacked together pretty quickly for shits and giggles. Makes me wish I'd used json instead of XML.

I'm only typically rendering one image at a time (although multiple cameras can be added to the scene file and it should render them all). However, it would be cool, in order to aid the generation of many images (perhaps for an animated camera path), or even just to divide the labour of a large/complex render, to have a basic master and agent services to allow basic render farming. Would be pretty trivial to implement.

It would be nice to add support for more sophisticated scenes - perhaps loading models straight from 3DS or FBX files (and all the texturing desirable with that also). Of course more geometry means more CPU work, but perhaps the increased complexity could be offset by some spatial partitioning to speed up the tracing.

I think that's part of the fun of ray tracing - there are lots of little things that can be done as quick projects -~~reflections~~/refraction, ~~soft shadows~~, animation, better illumination, parametric surfaces, etc. etc.

There just isn't enough hours in the day :)
