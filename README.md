# Sphinx-UE4
Sphinx-UE4 is a speech recognition plugin for Unreal Engine 4. 
The plugin makes use of the Pocketsphinx library.

<b>Note:</b> 
This plugin uses a slightly modified version of PocketSphinx, to support passing in a set of keywords, dynamically.
The modified code is contained within the _other folder.

This repo is forked form https://github.com/shanecolb/sphinx-ue4 and some other pieces of code form https://forums.unrealengine.com/t/plugin-speech-recognition/43703 
I am basically having some issues with Andriod build, I fixed most bugs and got it working with UE4 4.27.2 But after i have packaged it will stop detecting, works fine in debug mode. 

if anyone is able to resolve it please let me know 
ssshammi@gmail.com

I basically cleaned the code to make it work with unreal 4.27.2 and added the supported and missing files for andriod. but When i package it, it will not work, And i tried several things, But it seems that the java code is not getting initiated or it's the build settings. Also i had issues which OBB file not getting generated when changed to arm64. However when i migrated the map it would let me build for arm64. As i am missing skills in java andriod develelopment the It would help if someone is able to help with the issues or make it compatable with the latest andriod OS. 
