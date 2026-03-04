--SQL skript pro vytvoření základních objektů databábaze

--Jiří Kotek        xkotek09        xkotek09@vutbr.cz
--Dominink Honza    xhonza04        xhonza04@vutbr.cz

--Zadání: Informační systém pro letiště

--DROP
DROP TABLE CrewBoarded;
DROP TABLE BoardingTicket;
DROP TABLE Flight;
DROP TABLE Gate;
DROP TABLE Terminal;
DROP TABLE Plane;
DROP TABLE Users;

--CREATE

--Použita transformace číslo 4: "Všechno v jedné tabulce"
--Odůvodnění: Zjednodušení pro SELECT skripty, hodnoty budou vždy disjunktní (nemůže se stát, že bude nabývat hodnoty obou)
--Variant:
    --0 Traveller
    --1 Worker
    --2 Crew member
--Auth:
    --0 False
    --1 True
CREATE TABLE Users(
    UsersID         INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
    Name            VARCHAR (30)    NOT NULL,
    Surname         VARCHAR (30)    NOT NULL,
    Email           VARCHAR (60)    NOT NULL,
         CONSTRAINT email_format CHECK (email LIKE '%_@__%.__%'),
    PhoneNumber     VARCHAR (13)    NOT NULL,
        --CONSTRAINT phone_format CHECK (PhoneNumber LIKE '+[0-9]{12}'),
    DateOfBirth     DATE            NOT NULL,
    Nationality     VARCHAR (3)     NOT NULL,
    Street          VARCHAR (40)    NOT NULL,
    City            VARCHAR (40)    NOT NULL,
    Country         VARCHAR (40)    NOT NULL,
    Variant         SMALLINT,
    Auth            SMALLINT,
    HealthProblems VARCHAR (150),
    SecurityWorkerID INT,
    DocumentNumber  VARCHAR (30),
    WorkingPosition VARCHAR (30),
        --CONSTRAINT position_status CHECK (Position IN ('cleaner', 'salesman', 'security_worker')),
    IDAttestation   INT,
    Qualifications  VARCHAR (30),
        --Specify or not?
        --CONSTRAINT qualification_status CHECK (Qualifications IN ('pilot', 'steward', 'mechanic'))
    
    --Schválení bezpečnostním pracovníkem
    FOREIGN KEY (SecurityWorkerID) REFERENCES Users(UsersID) ON DELETE CASCADE
);


CREATE TABLE Terminal(
    TerminalID INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
    TerminalName VARCHAR (2),
    NumberOfGates SMALLINT NOT NULL,
    Capacity INT NOT NULL
);

CREATE TABLE Gate(
    GateID      INT NOT NULL,
    TerminalID  INT NOT NULL,
    GateType VARCHAR (9) NOT NULL,
    GateNumber  INT,
        --CONSTRAINT gate_type CHECK (stav IN ('transport', 'civil'))
    PRIMARY KEY (GateID, TerminalID), -- Composite primary key
    FOREIGN KEY (TerminalID) REFERENCES Terminal(TerminalID) ON DELETE CASCADE
);

--Použita transformace číslo 4: "Všechno v jedné tabulce"
--Odůvodnění: Zjednodušení pro SELECT skripty
--PlaneType:
    --0 Civil
    --1 Cargo
CREATE TABLE Plane(
    PlaneID INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
    PlaneType VARCHAR (9) NOT NULL,
        --CONSTRAINT plane_type CHECK (stav IN ('transport', 'civil')),
    Producer VARCHAR (30),
    AssemblyTime DATE, 
    LastRevision DATE,  
    NumberOfCrewMembers INT,
    MaximumLiftOffWeight NUMERIC (13,2),
    PlaneState VARCHAR (20),
    CargoType VARCHAR (50),
    CargoSpaceWidth NUMERIC (4,3),
    CargoSpaceLength NUMERIC (4,3),
    CargoSpaceHeight NUMERIC (4,3),
    RampSpaceWidth NUMERIC (4,3),
    RampSpaceLength NUMERIC (4,3),
    RampSpaceHeight NUMERIC (4,3),
    ChairConfiguration VARCHAR (100),
    NumberOfDoors INT,
    PassengerCapacity INT,
    Operand VARCHAR (100)
);

CREATE TABLE Flight (
    FlightID                INT GENERATED AS IDENTITY NOT NULL PRIMARY KEY,
    CoordinatorID           INT NOT NULL,
    TerminalID              INT NOT NULL,
    GateID                  INT NOT NULL,
    PlaneID                 INT NOT NULL,
    DepartureTime           TIMESTAMP       NOT NULL,
    EstimatedFlightTime     NUMERIC(4, 2)   NOT NULL,
    RealFlightTime          NUMERIC(4, 2),
    FlightDestination       VARCHAR(40)     NOT NULL,
    State_status            VARCHAR(30)     NOT NULL,
    Occupancy               INT,
    FlightSummary           VARCHAR(30),
    
    FOREIGN KEY (PlaneID) REFERENCES Plane(PlaneID) ON DELETE CASCADE, -- Plane whcih makes the flight
    FOREIGN KEY (CoordinatorID) REFERENCES Users(UsersID) ON DELETE CASCADE, -- Cordinator of the flight
    FOREIGN KEY (GateID,TerminalID) REFERENCES Gate(GateID,TerminalID) ON DELETE CASCADE -- Gate and terminal where the plane will be boarded
    --CONSTRAINT flight_state CHECK (State IN ('landed', 'takeoff', 'fly'))
);


CREATE TABLE BoardingTicket (
    UsersId INT NOT NULL,
    FlightId INT NOT NULL,
    BoardingTime TIMESTAMP,
    FlightTicketNmuber NUMERIC(4, 0),
    Seat VARCHAR (8) NOT NULL,
    SeatClass VARCHAR (9) NOT NULL,
    SeatType VARCHAR (3) NOT NULL,
    
    PRIMARY KEY (UsersID, FlightID), -- Composite primary key
    FOREIGN KEY (UsersID) REFERENCES Users(UsersID) ON DELETE CASCADE, -- Passenger got Board ticket ID
    FOREIGN KEY (FlightID) REFERENCES Flight(FlightID) ON DELETE CASCADE -- Flight for which is boarding ticket
    
    --CONSTRAINT flight_state CHECK (State IN ('landed', 'takeoff', 'fly'))
);

-- M:N Posádka letu
CREATE TABLE CrewBoarded (
    UsersID INT NOT NULL,
    FlightID INT NOT NULL,
    
    PRIMARY KEY (UsersID, FlightID), -- Composite primary key
    FOREIGN KEY (UsersID) REFERENCES Users(UsersID) ON DELETE CASCADE, -- Passenger ID
    FOREIGN KEY (FlightID) REFERENCES Flight(FlightID) ON DELETE CASCADE -- Flight ID
);

INSERT INTO Users (Name, Surname, Email, PhoneNumber, DateOfBirth, Nationality, Street, City, Country, Variant, Auth, HealthProblems, DocumentNumber) 
VALUES ('John', 'Boe', 'john.boe@example.com', '+123456789000', TO_DATE('1990-05-15', 'YYYY-MM-DD'), 'USA', '123 Main St', 'New York', 'USA', 0, 1, NULL, 'AB123456');

-- Crew1
INSERT INTO Users (Name, Surname, Email, PhoneNumber, DateOfBirth, Nationality, Street, City, Country, Variant, Auth, HealthProblems, DocumentNumber, WorkingPosition, IDAttestation, Qualifications) 
VALUES ('Emily', 'Clark', 'emily.clark@example.com', '+987654321001', DATE '1985-12-20', 'GBR', '456 Elm Street', 'London', 'UK', 1, 1, 'None', 'CD654321', 'Security Worker', 456, 'Security');

-- Crew2
INSERT INTO Users (Name, Surname, Email, PhoneNumber, DateOfBirth, Nationality, Street, City, Country, Variant, Auth, HealthProblems, DocumentNumber, WorkingPosition, IDAttestation, Qualifications) 
VALUES ('Alex', 'Johnson', 'alex.johnson@example.com', '+123098456007', DATE '1978-04-30', 'CAN', '789 Pine Road', 'Toronto', 'CAN', 0, 1, 'Allergy', 'EF123456', 'Salesman', 789, 'Sales');

-- Crew3
INSERT INTO Users (Name, Surname, Email, PhoneNumber, DateOfBirth, Nationality, Street, City, Country, Variant, Auth, HealthProblems, DocumentNumber, WorkingPosition, IDAttestation, Qualifications) 
VALUES ('Maria', 'Gonzalez', 'maria.gonzalez@example.com', '+654321789002', DATE '1992-07-11', 'USA', '1012 Maple Avenue', 'Miami', 'USA', 1, 0, 'Asthma', 'GH567890', 'Steward', 1012, 'Service');

-- Passenger1
INSERT INTO Users (Name, Surname, Email, PhoneNumber, DateOfBirth, Nationality, Street, City, Country, Variant, Auth, HealthProblems, DocumentNumber, WorkingPosition, IDAttestation, Qualifications) 
VALUES ('Jane', 'Smith', 'jane.smith@example.com', '+987654321000', TO_DATE('1995-10-20', 'YYYY-MM-DD'), 'UK', '456 Elm St', 'London', 'UK', 0, 1, NULL, 'CD987654', 'Steward', 456, 'Steward');

-- Termianl1
INSERT INTO Terminal (TerminalName, NumberOfGates, Capacity)
VALUES ('A', 5, 200);

-- Termianl2
INSERT INTO Terminal (TerminalName, NumberOfGates, Capacity)
VALUES ('B', 7, 250);

-- Termianl3
INSERT INTO Terminal (TerminalName, NumberOfGates, Capacity)
VALUES ('Z', 2, 400);

-- Gate1
INSERT INTO Gate (GateID, TerminalID, GateType, GateNumber)
VALUES (1, 1, 'civil', 1);

-- Gate2
INSERT INTO Gate (GateID, TerminalID, GateType, GateNumber)
VALUES (2, 1, 'civil', 2);

-- Gate3
INSERT INTO Gate (GateID, TerminalID, GateType, GateNumber)
VALUES (3, 2, 'cargo', 1);

-- Gate4
INSERT INTO Gate (GateID, TerminalID, GateType, GateNumber)
VALUES (4, 2, 'cargo', 2);

-- Gate5
INSERT INTO Gate (GateID, TerminalID, GateType, GateNumber)
VALUES (5, 3, 'military', 1);

-- Gate6
INSERT INTO Gate (GateID, TerminalID, GateType, GateNumber)
VALUES (6, 3, 'military', 2);


-- Insert sample data into Plane table
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Boeing', TO_DATE('2023-01-01', 'YYYY-MM-DD'), TO_DATE('2024-01-01', 'YYYY-MM-DD'), 5, 50000, 'Ready', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'A720', 3, NULL, 240);

INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Cargo', 'Airbus', TO_DATE('2022-06-01', 'YYYY-MM-DD'), TO_DATE('2023-12-01', 'YYYY-MM-DD'), 3, 75000, 'Maintenance', 'General Cargo', 4, 4, 4, 4, 4, 4, 'A320', 2, 'Operational', 0);

-- Plane 3
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Airbus', TO_DATE('2023-03-15', 'YYYY-MM-DD'), TO_DATE('2024-03-15', 'YYYY-MM-DD'), 6, 60000, 'Ready', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 350);

-- Plane 4
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Military', 'Lockheed Martin', TO_DATE('2022-08-20', 'YYYY-MM-DD'), TO_DATE('2023-12-20', 'YYYY-MM-DD'), 4, 80000, 'Maintenance', 'Tactical Equipment', 5, 5, 5, 5, 5, 5, 'C130', 3, 'Operational', 0);

-- Plane 5
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Boeing', TO_DATE('2022-10-10', 'YYYY-MM-DD'), TO_DATE('2023-10-10', 'YYYY-MM-DD'), 4, 70000, 'Maintenance', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 142);

-- Plane 6
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Embraer', TO_DATE('2023-02-28', 'YYYY-MM-DD'), TO_DATE('2024-02-28', 'YYYY-MM-DD'), 2, 45000, 'Ready', 'Luggage', 3, 3, 3, 3, 3, 3, 'E190', 2, 'Operational', 58);

-- Plane 7
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Cargo', 'Antonov', TO_DATE('2022-05-05', 'YYYY-MM-DD'), TO_DATE('2023-05-05', 'YYYY-MM-DD'), 5, 100000, 'Maintenance', 'Bulk Cargo', 6, 6, 6, 6, 6, 6, 'An-225', 4, 'Operational', 0);

-- Plane 8
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Bombardier', TO_DATE('2023-04-10', 'YYYY-MM-DD'), TO_DATE('2024-04-10', 'YYYY-MM-DD'), 3, 50000, 'Maintenance', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'CRJ900', 2, 'Operational', 540);

-- Plane 9
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Airbus', TO_DATE('2022-07-01', 'YYYY-MM-DD'), TO_DATE('2023-07-01', 'YYYY-MM-DD'), 6, 65000, 'Ready', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 140);

-- Plane 10
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Boeing', TO_DATE('2023-05-20', 'YYYY-MM-DD'), TO_DATE('2024-05-20', 'YYYY-MM-DD'), 1, 75000, 'Maintenance', 'Passenger Luggage', 4, 5, 4, 4, 5, 4, 'B737', 3, 'Operational', 2);

-- Plane 11
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Military', 'Lockheed Martin', TO_DATE('2022-11-11', 'YYYY-MM-DD'), TO_DATE('2023-11-11', 'YYYY-MM-DD'), 4, 90000, 'Ready', 'Arms and Ammunition', 7, 7, 7, 7, 7, 7, 'C-5M Super Galaxy', 4, 'Operational', 0);

-- Plane 12
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth,  RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Cargo', 'Embraer', TO_DATE('2023-01-15', 'YYYY-MM-DD'), TO_DATE('2024-01-15', 'YYYY-MM-DD'), 3, 80000, 'Maintenance', 'Special Cargo', 6, 8, 6, 6, 8, 6, NULL, 2, 'Operational', 0);


-- Insert sample data into Flight table
-- Flight 1
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 1, 10, TIMESTAMP '2024-07-05 10:00:00', 2.5, 2.45, 'New York', 'takeoff', 0);

-- Flight 2
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 1, 1, TIMESTAMP '2024-06-05 12:00:00', 3.0, 1.5, 'San Francisco', 'landed', 0);

-- Flight 3
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (2, 2, 3, 2, TIMESTAMP '2025-04-06 08:30:00', 2.0,8.0, 'Chicago', 'boarding', 0);

-- Flight 4
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (2, 1, 1, 3, TIMESTAMP '2024-04-07 15:45:00', 1.5, 16.0, 'Miami', 'check-in', 0);

-- Flight 5
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (2, 1, 2, 4, TIMESTAMP '2021-04-08 11:00:00', 2.75, 14.0, 'NewYork', 'gate closed', 0);

-- Flight 6
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (2, 1, 1, 5, TIMESTAMP '2024-9-09 09:15:00', 2.25, 1.0, 'Atlanta', 'delayed', 0);

-- Flight 7
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 2, 3, 6, TIMESTAMP '2024-04-10 13:30:00', 3.5, 0.5, 'Denver', 'canceled', 0);

-- Flight 8
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 2, 7, TIMESTAMP '2024-04-11 07:45:00', 1.75, 2.0, 'NewYork', 'boarding', 0);

-- Flight 9
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 2, 8, TIMESTAMP '2024-04-12 14:20:00', 3.25, 15.0, 'NewYork', 'check-in', 0);

-- Flight 10
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 2, 1, TIMESTAMP '2024-04-13 10:30:00', 2.0, 14.0, 'Houston', 'gate closed', 0);

-- Flight 11
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 2, 10, TIMESTAMP '2024-04-14 16:00:00', 2.5, 12.0, 'Atlanta', 'delayed', 0);

-- Flight 12
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 2, 4, 1, TIMESTAMP '2024-04-15 12:45:00', 3.75, 4.0, 'San Francisco', 'canceled', 0);

-- Flight 13
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (2, 2, 3, 3, TIMESTAMP '2024-04-16 09:30:00', 2.25, 2.25, 'NewYork', 'boarding', 0);

-- Flight 14
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (2, 1, 1, 4, TIMESTAMP '2024-04-17 11:15:00', 3.0, 3.5, 'NewYork', 'takeoff', 0);

-- Flight 15
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 1, 5, TIMESTAMP '2024-04-18 13:00:00', 2.5, 2.75, 'NewYork', 'landing', 0);

-- Flight 16
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 2, 7, TIMESTAMP '2024-05-01 09:45:00', 2.0, 2.25, 'NewYork', 'boarding', 0);

-- Flight 17
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 2, 4, 8, TIMESTAMP '2024-05-02 12:30:00', 2.75, 3.0, 'NewYork', 'takeoff', 0);

-- Flight 18
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (2, 1, 2, 9, TIMESTAMP '2024-05-03 14:15:00', 2.5, 2.75, 'NewYork', 'landing', 0);

-- Flight 19
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (2, 2, 3, 10, TIMESTAMP '2024-05-04 16:00:00', 3.0, 3.25, 'NewYork', 'gate closed', 0);

-- Flight 20
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 1, 1, TIMESTAMP '2024-05-05 18:45:00', 2.25, 2.5, 'NewYork', 'boarding', 0);

-- Insert sample data into Ticket table
-- Ticket 1
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (3, 1, TIMESTAMP '2024-03-02 06:00:00', 1121, '1A', 'Economy', 'AIS');

-- Ticket 2
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (2, 1, TIMESTAMP '2024-03-02 10:00:00', 1094, '1B', 'Economy', 'AIS');

-- Ticket 3
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (3, 3, TIMESTAMP '2024-03-02 10:00:00', 1871, '13B', 'Bussines', 'AIS');

-- Ticket 4
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (4, 3, TIMESTAMP '2024-03-02 16:30:00', 1094, '12A', 'Bussines', 'AIS');

-- Ticket 5
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (5, 4, TIMESTAMP '2024-03-02 16:30:00', 1871, '13B', 'Bussines', 'AIS');

-- Ticket 6
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (1, 2, TIMESTAMP '2024-03-03 06:00:00', 1121, '12A', 'Economy', 'AIS');

-- Ticket 7
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (5, 2, TIMESTAMP '2024-03-03 10:00:00', 1094, '12C', 'Bussines', 'AIS');

-- Ticket 8
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (1, 11, TIMESTAMP '2024-03-03 10:00:00', 1871, '3B', 'Economy', 'AIS');

-- Ticket 9
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (4, 5, TIMESTAMP '2024-03-03 16:30:00', 1094, '7A', 'Economy', 'AIS');

-- Ticket 10
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (1, 3, TIMESTAMP '2024-03-03 16:30:00', 1871, '13B', 'Bussines', 'AIS');

-- Ticket 11
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (1, 4, TIMESTAMP '2024-03-10 06:00:00', 1121, '12A', 'Economy', 'AIS');

-- Ticket 12
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (1, 8, TIMESTAMP '2024-03-10 10:00:00', 1094, '21A', 'Bussines', 'AIS');

-- Ticket 13
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (4, 4, TIMESTAMP '2024-03-10 10:00:00', 1871, '1B', 'Bussines', 'AIS');

-- Ticket 14
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (3, 9, TIMESTAMP '2024-03-10 16:30:00', 1094, '6A', 'Bussines', 'AIS');

-- Ticket 15
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (2, 3, TIMESTAMP '2024-03-10 16:30:00', 1871, '13B', 'Bussines', 'AIS');

-- Ticket 16
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (1, 1, TIMESTAMP '2024-03-11 06:00:00', 1121, '12A', 'Economy', 'AIS');

-- Ticket 17
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (2, 2, TIMESTAMP '2024-03-11 10:00:00', 1094, '12A', 'Bussines', 'AIS');

-- Ticket 18
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (2, 8, TIMESTAMP '2024-03-11 10:00:00', 1871, '73B', 'Bussines', 'AIS');

-- Ticket 19
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (2, 10, TIMESTAMP '2024-03-11 16:30:00', 1094, '72A', 'Economy', 'AIS');

-- Ticket 20
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (2, 6, TIMESTAMP '2024-03-11 16:30:00', 1871, '3B', 'Bussines', 'AIS');

-- Ticket 21
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (2, 9, TIMESTAMP '2024-03-11 10:00:00', 1094, '12A', 'Bussines', 'AIS');

-- Ticket 22
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (5, 3, TIMESTAMP '2024-03-11 10:00:00', 1871, '73B', 'Bussines', 'AIS');

-- Ticket 25
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (5, 10, TIMESTAMP '2024-03-11 16:30:00', 1094, '2A', 'Economy', 'AIS');

-- Ticket 26
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (5, 1, TIMESTAMP '2024-03-11 16:30:00', 1871, '3B', 'Bussines', 'AIS');

INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (1, 10, TIMESTAMP '2024-03-11 16:30:00', 1447, '2A', 'Economy', 'AIS');


INSERT INTO CrewBoarded (UsersID, FlightID)
VALUES (3, 1);

INSERT INTO CrewBoarded (UsersID, FlightID)
VALUES (4, 1);

-- Dotaz 1: Spojení dvou tabulek - Výpis letů a odpovídajících informací o letadle
-- Funkce: Načítá podrobnosti každého letu spolu s typem letadla a jeho stavem.
SELECT F.FlightID, F.FlightDestination, P.PlaneType, P.PlaneState
FROM Flight F
JOIN Plane P ON F.PlaneID = P.PlaneID;

-- Dotaz 2: Spojení dvou tabulek - Hledání všech pasažérů pro konkrétní let
-- Funkce: Vypisuje všechny pasažéry zarezervované na konkrétní let, užitečné pro správu posádek a sestavy pasažérů.
SELECT U.Name, U.Surname, B.FlightID
FROM Users U
JOIN BoardingTicket B ON U.UsersID = B.UsersID
WHERE B.FlightID = 1; -- Příklad pro letové ID 1

-- Dotaz 3: Spojení tří tabulek - Informace o letu, bráně a terminálu pro každý let
-- Funkce: Poskytuje komplexní informace o odjezdu letů, usnadňuje navigaci a plánování pro cestující a personál.
SELECT F.FlightID, F.FlightDestination, G.GateNumber, T.TerminalName
FROM Flight F
JOIN Gate G ON F.GateID = G.GateID
JOIN Terminal T ON G.TerminalID = T.TerminalID;

-- Dotaz 4: Použití GROUP BY a agregace - Počet letů podle destinace
-- Funkce: Shromažďuje lety podle destinace k analýze provozu a popularity destinací.
SELECT FlightDestination, COUNT(*) AS NumberOfFlights
FROM Flight
GROUP BY FlightDestination;

-- Dotaz 5: Použití GROUP BY a agregace - Průměrný skutečný čas letu podle typu letadla
-- Funkce: Vypočítává průměrný skutečný čas letu pro každý typ letadla, užitečné pro analýzu výkonu a plánování.
SELECT P.PlaneType, CAST(AVG(F.RealFlightTime) AS DECIMAL(10,3)) AS AverageFlightTime
FROM Flight F
JOIN Plane P ON F.PlaneID = P.PlaneID
GROUP BY P.PlaneType;

-- Dotaz 6: Použití EXISTS predikátu - Lety s naloženou posádkou
-- Funkce: Identifikuje lety, které mají alespoň jednoho člena posádky na palubě, zajistí dodržování bezpečnostních předpisů.
SELECT F.FlightID, F.FlightDestination
FROM Flight F
WHERE EXISTS (SELECT 1 FROM CrewBoarded C WHERE F.FlightID = C.FlightID);

-- Dotaz 7: Použití IN s vnořeným SELECTem - Pasažéři, kteří nastoupili na konkrétní lety
-- Funkce: Najde pasažéry, kteří nastoupili na lety určené do konkrétní destinace, užitečné pro zákaznický servis a sledování pasažérů.

SELECT U.Name, U.Surname
FROM Users U
WHERE U.UsersID IN (
SELECT B.UsersID
FROM BoardingTicket B
JOIN Flight F ON B.FlightID = F.FlightID
WHERE F.FlightDestination IN ('New York', 'Los Angeles') -- Příklad destinací
);

-- EXTRA DOTAZY

-- Dotaz 8+: Počítání počtu letů, které každé letadlo absolvovalo
-- Funkce: Tento dotaz pomáhá určit, jak často je každé letadlo používáno, což může být klíčové pro plánování údržby, operační plánování a porozumění míře využití flotily.
SELECT PlaneID, COUNT(*) AS NumberOfFlights
FROM Flight
GROUP BY PlaneID;

-- Dotaz 9+: Identifikace letadla s maximální vzletovou hmotností ve flotile
-- Funkce: Užitečné pro správu nákladu a plánování, tento dotaz identifikuje letadlo s nejvyšší kapacitou pro přepravu hmotnosti, což může být rozhodující pro rozhodnutí o nákladních letech nebo přepravě těžkých břemen.

SELECT PlaneID, Producer, MAX(MaximumLiftOffWeight) AS MaxLiftOffWeight
FROM Plane
GROUP BY PlaneID,Producer,MaximumLiftOffWeight
HAVING MaximumLiftOffWeight > ALL(SELECT PlaneID FROM Plane)
ORDER BY MaximumLiftOffWeight DESC;
SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);

-- Dotaz 10+: Shromažďování prodeje letenek podle třídy pro každý let
-- Funkce: Tento dotaz nabízí náhled na preference pasažérů a trendy v rezervacích, usnadňuje správu příjmů a zlepšení služeb tím, že zdůrazňuje nejoblíbenější třídy sedadel mezi cestujícími.
SELECT FlightID, SeatClass, COUNT(*) AS TicketsSold
FROM BoardingTicket
GROUP BY FlightID, SeatClass;

-- Dotaz 11+: Podmíněná agregace - Výpočet průměrného času letu pro lety ve specifickém stavu, užitečné pro statistiky pro koordinátora letu
SELECT State_status, AVG(RealFlightTime) AS AvgFlightTime
FROM Flight
GROUP BY State_status;

-- Dotaz 12+: Agregace - Počítání počtu pasažérů na každém letu
SELECT Flight.FlightID, COUNT(BoardingTicket.UsersID) AS NumPassengers
FROM Flight
LEFT JOIN BoardingTicket ON Flight.FlightID = BoardingTicket.FlightID
GROUP BY Flight.FlightID;


-- View pro zobrazení letů kordinátor letů
DROP VIEW OlderThanWeekFlights;
CREATE VIEW OlderThanWeekFlights AS
SELECT *
FROM Flight
WHERE DepartureTime < SYSTIMESTAMP - INTERVAL '7' DAY;

-- Pro testování správnosti výběru view
-- SELECT * FROM OlderThanWeekFlights;

-- View pro zobrazení nadcházejících letů pro všechny uživatele systému
DROP VIEW UpcomingFlightsView;
CREATE VIEW UpcomingFlightsView AS
SELECT *
FROM Flight
WHERE DepartureTime > CURRENT_TIMESTAMP;

-- Pro testování správnosti výběru view
-- SELECT * FROM UpcomingFlightsView;

-- View pro zobrazení dostupných letadel k přiřazení k letu
DROP VIEW AvailablePlanesView;
CREATE VIEW AvailablePlanesView AS 
SELECT *
FROM Plane
WHERE LastRevision > SYSTIMESTAMP - INTERVAL '1' YEAR
AND Operand = 'Operational' AND Planestate = 'Ready';

-- Pro testování správnosti výběru view
-- SELECT * FROM AvailablePlanesView;

--Trigger pro kontrolu kapacity letadla při přidání nového letu
CREATE OR REPLACE TRIGGER check_plane_capacity
BEFORE INSERT ON Flight -- Assuming you want to check before inserting a new row in `Flight`
FOR EACH ROW
DECLARE
  v_capacity INT;
  v_num_passengers INT;
BEGIN
  -- Zachytí maximální kapacitu
  SELECT PassengerCapacity INTO v_capacity FROM Plane WHERE PlaneID = :NEW.PlaneID;

  -- Zachytí počet aktuálních přiřazených pasažerů
  SELECT COUNT(*) INTO v_num_passengers FROM BoardingTicket WHERE FlightID = :NEW.FlightID;

  -- Kontrola jestli není letadlo přetíženo
  IF v_capacity < v_num_passengers THEN
    RAISE_APPLICATION_ERROR(-20001, 'This plane does not have enough capacity for the flight.');
  END IF;
END;
/


-- Trigger pro aktualizaci stavu letu, Pokud se aktualizauje stav letu tak například kvůli výbuchu sopky 
-- dojde ke zrušení a všem pasažérům se zašle oznámení o zrušení letu, mohlo by být doimplementováno například notifikace posádky

CREATE OR REPLACE TRIGGER UpdateFlightStatus
AFTER UPDATE ON Flight
FOR EACH ROW
BEGIN
  IF :NEW.State_status = 'canceled' THEN
    -- Notifikace pro všechny pasažéry, které lze implementovat.
    DBMS_OUTPUT.put_line('Flight ' || :NEW.FlightID || ' has been canceled.');
  END IF;
END;
/

DROP MATERIALIZED VIEW AvailableFlights;
--Vytvořím materiální pohled AvailableFlights pro zobrazení dostupných letů, který bude patřit druhému členu týmu.
CREATE MATERIALIZED VIEW AvailableFlights
AS
SELECT FlightID, FlightDestination, DepartureTime
FROM Flight
WHERE State_status NOT IN ('canceled', 'delayed');

--  Ukázka přiřazení práv k pohledu AvailableFlights uživateli xhonza04
--  GRANT SELECT ON AvailableFlights TO xhonza04;

DROP VIEW OlderThanWeekFlights;
CREATE VIEW OlderThanWeekFlights AS
SELECT *
FROM Flight
WHERE DepartureTime < SYSTIMESTAMP - INTERVAL '7' DAY;

-- Pro testování správnosti výběru view
-- SELECT * FROM OlderThanWeekFlights;

-- View pro zobrazení nadcházejících letů pro všechny uživatele systému
DROP VIEW UpcomingFlightsView;
CREATE VIEW UpcomingFlightsView AS
SELECT *
FROM Flight
WHERE DepartureTime > CURRENT_TIMESTAMP;

-- Pro testování správnosti výběru view
-- SELECT * FROM UpcomingFlightsView;

-- View pro zobrazení dostupných letadel k přiřazení k letu
DROP VIEW AvailablePlanesView;
CREATE VIEW AvailablePlanesView AS 
SELECT *
FROM Plane
WHERE LastRevision > SYSTIMESTAMP - INTERVAL '1' YEAR
AND Operand = 'Operational' AND Planestate = 'Ready';

-- Pro testování správnosti výběru view
-- SELECT * FROM AvailablePlanesView;

-- Procedura aktualizuje terminál a gate letu, kontroluje zdali nebyl nový terminál stejný jako ten původní. Při úspěchu dodatečně pomocí kurzoru vypíše celou posádku ( možnost notifikace o změně)
-- Podle zadání využívá typ sloupce a kurzor
CREATE OR REPLACE PROCEDURE MoveFlightToTerminal(
    p_FlightID INT,
    p_NewTerminalID INT,
    p_NewGateID INT
)
AS
    v_OldTerminalID INT;
    v_OldGateID INT;
    v_CrewCursor SYS_REFCURSOR;
    v_CrewMember Users%ROWTYPE;
BEGIN
    -- Získání aktuálního terminálu letu
    SELECT TerminalID, GateID INTO v_OldTerminalID, v_OldGateID
    FROM Flight
    WHERE FlightID = p_FlightID;
    
    -- Pokud nový a starý terminál nejsou shodné
    IF v_OldTerminalID <> p_NewTerminalID THEN
        -- Aktualizace terminálu a brány letu
        UPDATE Flight
        SET TerminalID = p_NewTerminalID, GateID = p_NewGateID
        WHERE FlightID = p_FlightID;

        -- Výpis všech členů posádky pro tento let
        OPEN v_CrewCursor FOR
        SELECT u.*
        FROM Users u
        JOIN CrewBoarded cb ON u.UsersID = cb.UsersID
        WHERE cb.FlightID = p_FlightID;

        DBMS_OUTPUT.PUT_LINE('Flight successfully moved from ('|| v_OldTerminalID ||'-'|| v_OldGateID ||') to ('|| p_NewTerminalID||'-'|| p_NewGateID ||').');

        -- Vypsání členů posádky
        LOOP
            FETCH v_CrewCursor INTO v_CrewMember;
            EXIT WHEN v_CrewCursor%NOTFOUND;
            DBMS_OUTPUT.PUT_LINE('Crew member: ' || v_CrewMember.Name || ' ' || v_CrewMember.Surname);
        END LOOP;

        CLOSE v_CrewCursor;
    ELSE
        DBMS_OUTPUT.PUT_LINE('Flight cannot be moved to the same terminal.');
    END IF;
EXCEPTION
    WHEN NO_DATA_FOUND THEN
        DBMS_OUTPUT.PUT_LINE('Flight with ID ' || p_FlightID || ' not found.');
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('An error occurred while moving the flight.');
END;
/

-- Ukázka procedury aktulizace terminálu a gatu letu

-- Dostupne terminaly-gaty Osobni 1-1 a 1-2
-- Dostupne terminaly-gaty Nakladni 2-3 a 2-4
-- Dostupne terminaly-gaty Vojenske 3-5 a 3-6

-- Aktualizace terminálu pro let ID 1 z 1-1 na 1-2, očekávaný úspěch, přidány selecty pro kontrolu přesunutí

--SELECT FlightID, TerminalID, GateID FROM Flight WHERE FlightID = 1;
BEGIN
    MoveFlightToTerminal(1, 2, 4);
END;
/
--SELECT FlightID, TerminalID, GateID FROM Flight WHERE FlightID = 1;

-- Aktualizace terminálu pro let ID 1 z 2-4 na 2-4, očekávaný neúspěch nelze přesunout na stejný terminál
BEGIN
    MoveFlightToTerminal(1, 2, 4);
END;
/

-- Aktualizace terminálu pro let ID 1 z 2-4 na 2-4, očekávaný neúspěch neexistující let ani terminál
BEGIN
    MoveFlightToTerminal(141, 4, 4);
END;
/

---------------------------OPTIMALIZACE POMOCÍ INDEXU-----------------
-- Zde je optimalizace vlemi nízká, složitost vazeb je nízká. Optimalizace by byla výraznější u tabulek s složitějšími a hlubšími vazbami nebo podmínkami vyhledávání.
DROP INDEX idx_flight_destination; -- Drop klíče se chová zvláštně ručně DROP jde, v celém skriptu ne
EXPLAIN PLAN FOR
SELECT U.Name, U.Surname
FROM Users U
WHERE U.UsersID IN (
    SELECT B.UsersID
    FROM BoardingTicket B
    JOIN Flight F ON B.FlightID = F.FlightID
    WHERE F.FlightDestination IN ('New York', 'Los Angeles')
);

SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);

CREATE INDEX idx_flight_destination ON Flight(FlightDestination);
EXPLAIN PLAN FOR
SELECT U.Name, U.Surname
FROM Users U
WHERE U.UsersID IN (
    SELECT B.UsersID
    FROM BoardingTicket B
    JOIN Flight F ON B.FlightID = F.FlightID
    WHERE F.FlightDestination IN ('New York', 'Los Angeles')
);
SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);
/*
TEORIE:
Plný přístup k tabulce FLIGHT:
    Původní plán: Přístup k tabulce FLIGHT byl proveden pomocí plného přístupu (TABLE ACCESS FULL), což není optimální, pokud máme filtrovat lety podle destinace.
    Optimalizovaný plán: Místo toho jsme použili index na sloupec FLIGHTDESTINATION v tabulce FLIGHT, což umožnilo efektivnější přístup k řádkům podle destinace letu.
Nesystematický přístup k tabulce FLIGHT pomocí IN-listu:
    Původní plán: Přístup k tabulce FLIGHT byl nesystematický pomocí IN-listu, což může být neefektivní, pokud máme velký počet hodnot pro filtrování.
    Optimalizovaný plán: Použili jsme index na sloupec FLIGHTDESTINATION, což umožnilo systematický přístup k řádkům podle destinace letu.

PRAXE: Na naší sadě dat je rozdíl těžko pozorovatelný a rozdíl by se prohjevil u vyššího počtu řádků    
*/
---------------------------OPTIMALIZACE POMOCÍ INDEXU KONEC-----------------

---------------------- INKREMENTACE OBSAZENOSTI LETU ----------------------
-- Funkce kontroluje, zda není obsazenost větší než kapacita letadla
CREATE OR REPLACE PROCEDURE INCREMENT_FLIGHT_OCCUPANCY(
p_FlightID INT
)
AS
v_ActualOccupancy Flight.Occupancy%TYPE;
v_PlaneCapacity Plane.PassengerCapacity%TYPE ;
BEGIN
SELECT Occupancy INTO v_ActualOccupancy FROM Flight WHERE FlightID = p_FlightID;
SELECT PassengerCapacity INTO v_PlaneCapacity
FROM Flight f
JOIN Plane p ON f.PlaneID = p.PlaneID
WHERE f.FlightID = p_FlightID;
-- Inkrementace obsazenosti, pokud je k dispozici kapacita
IF v_ActualOccupancy < v_PlaneCapacity THEN
UPDATE Flight
SET Occupancy = v_ActualOccupancy + 1
WHERE FlightID = p_FlightID;
ELSE
-- Vyjimka, když je letadlo již na maximální kapacitě
RAISE_APPLICATION_ERROR(-20001, 'Letadlo již dosáhlo maximální kapacity.');
END IF;
EXCEPTION
WHEN NO_DATA_FOUND THEN
-- Výjimkla, pokud není nalezeno FlightID
RAISE_APPLICATION_ERROR(-20002, 'FlightID nenalezeno.');
END;
/

-- Spouštěč pro inkrementaci obsazenosti letu
CREATE OR REPLACE TRIGGER increment_flight_occupancy_trigger
BEFORE INSERT ON BoardingTicket
FOR EACH ROW
DECLARE
BEGIN
INCREMENT_FLIGHT_OCCUPANCY(:new.FlightID);
END;
/

-- Let 10 má pouze 2 sedadla
-- Pokus o vložení 3 letenek na let
/*
-- Let 13
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 1, 2, 10, TIMESTAMP '2024-04-14 16:00:00', 2.5, 12.0, 'Atlanta', 'delayed', 0);

-- Letenka 28 0/2 OK
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (2, 13, TIMESTAMP '2024-03-11 16:30:00', 2475, '1A', 'Standart', 'ARR');
-- Letenka 29 1/2 OK
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (1, 13, TIMESTAMP '2024-03-11 16:30:00', 7514, '1B', 'Standart', 'ARR');
-- Letenka 30 2/2 FAIL
INSERT INTO BoardingTicket (UsersID, FlightID, BoardingTime, FlightTicketNmuber, Seat, SeatClass, SeatType)
VALUES (3, 13, TIMESTAMP '2024-03-11 16:30:00', 4492, '1C', 'Standart', 'ARR');
*/
---------------------- KONEC INKREMENTACE OBSAZENOSTI LETU ----------------------

---------------------- Kontrola revize LETADLA ----------------------
CREATE OR REPLACE TRIGGER check_plane_inspection
BEFORE INSERT OR UPDATE ON Flight
FOR EACH ROW
DECLARE
TimeDiff NUMBER;
MaxRevisionTime NUMBER := 365;
BEGIN
-- Vypočítat rozdíl v čase v dnech mezi aktuálním datem a poslední revizí letadla
SELECT (SYSDATE - p.LastRevision) INTO TimeDiff
FROM Plane p
WHERE p.PlaneID = :NEW.PlaneID;
IF TimeDiff > MaxRevisionTime THEN
    -- Vyvolá výjimku oznamující, že poslední revize letadla je starší než MaxRevisionTime
    RAISE_APPLICATION_ERROR(-20001, 'Poslední revize letadla je starší než 1 rok. Nelze vložit nebo aktualizovat let.');
END IF;
END;
/

-- Testovací případy pro spouštěč
/*
-- Letadlo 13 revize OK
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth, RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Boeing', TO_DATE('2023-05-20', 'YYYY-MM-DD'), TO_DATE('2024-05-20', 'YYYY-MM-DD'), 1, 76000, 'Maintenance', 'Passenger Luggage', 4, 5, 4, 4, 5, 4, 'B737', 3, 'Operational', 2);

-- Letadlo 14 revize FAIL
INSERT INTO Plane (PlaneType, Producer, AssemblyTime, LastRevision, NumberOfCrewMembers, MaximumLiftOffWeight, PlaneState, CargoType, CargoSpaceWidth, CargoSpaceLength, CargoSpaceHeight, RampSpaceWidth, RampSpaceLength, RampSpaceHeight, ChairConfiguration, NumberOfDoors, Operand, PassengerCapacity)
VALUES ('Civil', 'Boeing', TO_DATE('2020-01-20', 'YYYY-MM-DD'), TO_DATE('2022-03-02', 'YYYY-MM-DD'), 1, 76000, 'Maintenance', 'Passenger Luggage', 4, 5, 4, 4, 5, 4, 'B737', 3, 'Operational', 2);

-- Let 13 OK
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 2, 4, 13, TIMESTAMP '2024-04-15 12:45:00', 3.0, 4.0, 'Berlin', 'check-in', 0);

-- Let 14 FAIL
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 2, 4, 14, TIMESTAMP '2024-04-15 13:45:00', 3.75, 4.0, 'Prague', 'check-in', 0);
*/



---------------------- KONEC Kontroly revize letadla----------------------



--------------------------------------------    SWITCH PRIKLAD    --------------------------------------------
-- Ukazkova data
-- Flight 15
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 2, 4, 1, TIMESTAMP '2024-04-15 12:45:00', 3.75, 4.0, 'San Francisco', 'landed', 0);
-- Flight 16
INSERT INTO Flight (CoordinatorID, TerminalID, GateID, PlaneID, DepartureTime, EstimatedFlightTime, RealFlightTime, FlightDestination, State_status, Occupancy)
VALUES (1, 2, 4, 1, TIMESTAMP '2024-04-15 12:45:00', 3.75, 6.0, 'San Francisco', 'landed', 0);


-- Dotaz ziskava data o zpozdeni letu a vysledek zapise do polozky "FlightSummary"
WITH FlightSummary AS (
    SELECT FlightID,
        CASE 
            WHEN (RealFlightTime - EstimatedFlightTime) <= 0        -- Letadlo dorazilo s předstihem
                THEN 'Dorazilo s předstihem'
            WHEN (RealFlightTime - EstimatedFlightTime) <= 0.25     -- Letadlo dorazilo s minimálním zpožděním
                THEN 'Dorazilo s minimálním zpožděním'
            WHEN (RealFlightTime - EstimatedFlightTime) <= 0.5      -- Letadlo dorazilo s malým zpožděním
                THEN 'Dorazilo s malým zpožděním'
            WHEN (RealFlightTime - EstimatedFlightTime) <= 1        -- Letadlo dorazilo s přijatelným zpožděním
                THEN 'Dorazilo s přijatelným zpožděním'
            WHEN (RealFlightTime - EstimatedFlightTime) > 1         -- Letadlo dorazilo s velkým zpožděním
                THEN 'Dorazilo s velkým zpožděním'
            ELSE
               'Data nejsou k dispozici'
        END AS FlightSummary,
        RealFlightTime,
        EstimatedFlightTime,
        State_status
    FROM Flight
)
SELECT 
    FlightID,
    EstimatedFlightTime,
    RealFlightTime,
    FlightSummary,
    State_status
FROM 
    FlightSummary
WHERE 
    State_status = 'landed';


--------------------------------------------    KONEC SWITCH Priklad    --------------------------------------------