# Game 3: Low-Level Programming
#### Your task is to create a video game version of your table top game designed in the Play and Game module.
 
Tips:
- Ensure you store all code for server logic inside the server source directory. You can find the cmake file for adding cpp and hpp files in the cmake folder.
- Ensure you store all code for client logic inside the client source directory. You can find the cmake file for adding cpp and hpp files in the cmake folder. 
- All code that is common to both projects should go in the GameLib library. This is a static library that will get linked into your applications.
- Both applications will auto-link to the enet library
- Only the client application is set to link to ASGE and SoLoud