# Social Network Management System in C

This project is a Social Network Management System implemented in C, offering a comprehensive set of features for managing user relationships within a social network. The system provides a command-line interface for users to interact with various functionalities, including user authentication, friend management, and graph visualization.

# Features

## User Authentication and Authorization:

- Friendship Management:
- Add and remove friends from a user's network.
  View the list of current friends for a specific user.
  Check if two users are friends and display mutual connections.
- Graph Visualization:
  Generate a graphviz-compatible .DOT file for visualizing the social network graph.
  Display the social network graph using external tools like Graphviz.
  Friend Score Calculation:
- Calculate a friend score for each user based on their friend relationships.
- Securely manage user accounts with unique user IDs, usernames, and passwords.
  Differentiate between regular users and administrators for enhanced security.
## Admin Functionalities:

- Admins can perform actions such as displaying the adjacency matrix, opening data files, and finding user IDs.
- File Handling:
  Read and write user data to files, facilitating persistence across program runs.
  Handle file operations for adding, removing, and updating user and friendship data.
- Utilize BFS algorithm to find paths and mutual friends between two users.
- Interactive Command Line Menu
