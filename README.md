AFM Viz Tool
==================

Atomic Force Microscropy Visualization Tool

This tool was developed as a part of one of my senoir undergrad classes at UAB. Its purpose is to visulize the surface the machine scans. The Atomic Force Microscorpy (AFM) is a tool for mapping really small surfaces. This program breaks them down into a scalable object, or a 2D visualization of the data in some way.

There is an abstract and a presentation included along in the git. The abstract explains some of the techniques used in the program and about about how it works. The presentation dosent provide much info without me talking behind it, but it provides a few more examples of output as the program progressed. Other wise I will provide a breif descirption of the toolset here.

The program breaks down into 3 seperate executables and they are nested in their individual subdirectories.
	
	- 3-D
	- Publication
	- Analysis

You can see an example output image of each in this directory. The 3-D Images have bothe curved and non curved examples. Based on your AFM machine or your surface you may have to remove a curve if you want to analize data. The tool performs this surfaces flatening as an option in all of the visualizations, but the purpose of the 3-D visualization is often to allow the viewer to see the signifiganace of the curve or the shape of the surface in general.

Analysis
===================
The analysis tool allows you to see the particle height in a histogram alongside a visualization of the surface from the top looking down as it would be. allows for general analysis of particles on the surface from the scan.

Publication
===================
This produces a high resolution image of the surface with emulated light veiwed from above. This allows for a viewer to see the particles scattered about the surface, and is meant for producing images for publication.

3-D
===================
This progarm simply produces a hieght map in a viewable window that shows the surface. The user can apply various mathmatical affect to the surface and see how they affect the data. Most importantly a user can see the curve in the surface, and how removing that curve affects the data.