## Game Viewer

Game Viewer is a simple cross-platform client-server application for browsing a database of video games. The project is written in C++ with Qt and serves as a learning exercise.

### Features

	•	Client:
	•	Displays games as cards with pagination.
	•	Supports filtering, searching, and showing game covers.
	•	Responsive UI with a 9:16 aspect ratio.
	•	Server:
	•	Uses PostgreSQL as the database.
	•	Handles requests via WebSocket and JSON.
	•	Multithreaded for performance.
	•	Technologies:
	•	C++, Qt, PostgreSQL, WebSocket, JSON, Docker, CMake.

### How to Build and Run

	1.	Clone the repository:

git clone https://github.com/<your-username>/game-viewer.git  
cd game-viewer  


	2.	Build the project with CMake:

mkdir build && cd build  
cmake ..  
make  


	3.	Run the server using Docker:

docker-compose up  


	4.	Launch the client:

./GameViewer  
