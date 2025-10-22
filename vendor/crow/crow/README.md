# Charging C++ Backend

This project is a C++ backend service for managing and operating a network of electric vehicle (EV) charging stations. It provides a RESTful API for administrative tasks and for users to control charging sessions.

It is built using the [Crow C++ micro-framework](https://github.com/CrowCpp/Crow) for the web server, SQLite for the database, and `jwt-cpp` for handling JSON Web Token (JWT) based authentication.

## Features

*   **Station Management**: Add new charging stations and list existing ones.
*   **Outlet Management**: Add new outlets to stations and list all available outlets.
*   **User Management**: Register new users.
*   **Charging Control**: Initiate and terminate charging sessions for authenticated users.
*   **Transactional Integrity**: Ensures that checking for and occupying an outlet is an atomic operation to prevent race conditions.
*   **Health Check**: A simple endpoint to verify service health.

## Dependencies

*   **OpenSSL**: For cryptographic functions used by JWT.
*   **SQLite3**: For the database.
*   **libsodium**: For hashing and other cryptographic needs.

The project also includes the following vendored libraries:
*   **Crow**: Web framework.
*   **ASIO**: Networking library (used by Crow).
*   **jwt-cpp**: For creating and validating JWTs.

## Building the Project

The project uses CMake. To build it, you can run the following commands from the project root:

```bash
mkdir build
cd build
cmake ..
make
```

This will generate an executable named `charging` in the `build` directory.

## API Endpoints

The following are the available API endpoints.

### Health Check

*   **GET** `/health`

    A simple health check endpoint.

    **Success Response (200 OK):**
    ```
    Service is healthy
    ```

### Management API

These endpoints are for administrative purposes.

*   **POST** `/management/stations`

    Adds a new charging station.

    **Request Body:**
    ```json
    {
      "name": "Central City Station",
      "location": "123 Main St"
    }
    ```

*   **GET** `/management/stations`

    Retrieves a list of all charging stations.

*   **POST** `/management/outlets`

    Adds a new outlet to an existing station. The status is automatically set to `available`.

    **Request Body:**
    ```json
    {
      "station_id": 1,
      "name": "Outlet A1"
    }
    ```

*   **GET** `/management/outlets`

    Retrieves a list of all outlets across all stations.

*   **POST** `/management/users`

    Registers a new user.

    **Request Body:**
    ```json
    {
      "name": "John Doe",
      "email": "john.doe@example.com",
      "password": "securepassword123"
    }
    ```

### Charging API

These endpoints are for users to control charging sessions. They require a valid JWT for authentication.

*   **POST** `/charging/initiate`

    Starts a charging session on a specific outlet. The service ensures the outlet is available and marks it as occupied in a single database transaction.

    **Request Body:**
    ```json
    {
      "outlet_id": 5
    }
    ```

*   **POST** `/charging/terminate`

    Stops a charging session and makes the outlet available again.

    **Request Body:**
    ```json
    {
      "outlet_id": 5
    }
    ```

