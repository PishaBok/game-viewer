# tcpserver

## Game Library Manager

#### Overview

Game Library Manager is a client-server application built using C++/Qt and PostgreSQL, designed to manage a personal game library. The project serves as an educational endeavor to explore modern software development concepts, including client-server architecture, web sockets, multithreading, and GUI design.

#### Features

	•	User Authentication: Login and registration functionality to ensure personalized access.
	•	Game Database:
	•	Browse a shared database of ~10,000 games, with search and filtering capabilities.
	•	View details and navigate through paginated game lists.
	•	Personal Library:
	•	Save games of interest to a personal library, similar to bookmarks.
	•	Switch between the shared database and the personal library via a dual-menu interface.
	•	Real-time Communication: Operates via WebSockets for fast and interactive data exchange.
	•	Multithreading: Efficient handling of client-server operations using both std::thread and Qt’s threading framework.

#### Technology Stack

	•	Frontend:
	•	C++ with Qt for a responsive and user-friendly graphical interface.
	•	Backend:
	•	PostgreSQL as the database for handling game and user data.
	•	Custom-built server in C++ to manage requests, filter data, and serve results.
	•	Communication Protocol: WebSockets for low-latency, bidirectional communication.

#### Database Structure

The application utilizes a PostgreSQL database with three primary tables:

	1.	game: Contains the main game catalog (~10,000 entries).
		Fields: id, name, platform, genre, release_year, etc.
	2.	user: Stores user login credentials.
		Fields: id, username, password.
	3.	usergame: Manages relationships between users and their personal game libraries.
		Fields: user_id, game_id.

#### Purpose

This project was created as a learning experience to dive deeper into:
	•	Client-server communication using WebSockets.
	•	Managing a complex database structure with PostgreSQL.
	•	Developing responsive and scalable GUI applications.
	•	Utilizing multithreading to enhance application performance.

#### Future Improvements

	•	Add a recommendation system based on user preferences.
	•	Extend database functionality to include more detailed game statistics and user reviews.
	•	Implement cloud synchronization for user libraries.

#### How to Run

	1.	Server:
		Build the server application using CMake and run it on a machine with PostgreSQL installed.
		Ensure the WebSocket port is correctly configured and accessible.
	2.	Client:
		Build the client application using QtCreator or your preferred C++ build system.
		Connect to the server via WebSocket by providing the appropriate URL.
	3.	Database:
		Populate the database with the provided schema and sample data.

Feel free to explore and extend the project to suit your needs! 😊
