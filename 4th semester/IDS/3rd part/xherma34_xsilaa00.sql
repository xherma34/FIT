drop table DoctorDepartement;
drop table PatientMedication;
drop table HospitalizationRoom;
drop table DoctorMedication;
drop table Hospitalizations;
drop table Medications;
drop table Rooms;
drop table Departements;
drop table Patients;
drop table Nurses;
drop table Doctors;
drop table Receptionists;
drop table Personel;

create table Personel(
PersonalNumber int generated as identity not null primary key,
FirstName varchar(128) not null,
LastName varchar(128) not null,
PhoneNumber varchar(128) not null,
--Opravneni se zatoulalo, misto nej byla pridana specializace k lekari a sestre
BirthDate date not null,
--Atomizace adresy bydliste
City varchar(128) not null,
PostCode varchar(128) not null,
Address varchar(128) not null,
Email varchar(128) not null
    check(regexp_like(Email, '^[a-z]+[a-z0-9.!#_*+-=]*@[a-z0-9]+[.][a-z]+$'))
);

create table Receptionists(
    ReceptionistID int generated as identity not null primary key,
    PersonalNumber int not null,
    constraint ReceptionistFK
    foreign key (PersonalNumber) references Personel(PersonalNumber)
);

create table Doctors(
    DoctorID int generated as identity not null primary key,
    PersonalNumber int not null,
    constraint DoctorFK
    foreign key (PersonalNumber) references Personel(PersonalNumber),
    Specialization varchar(128) not null
);

create table Nurses(
    NurseID int generated as identity not null primary key,
    PersonalNumber int not null,
    constraint NurseFK
    foreign key(PersonalNumber) references Personel (PersonalNumber),
    Specialization varchar(128) not null
);

create table Patients(
--RÈ má maximálnì 10 znakù bez lomítka a je dìlitelné 11
NationalID char(10) not null primary key
    check(mod (NationalID,11) = 0), 
FirstName varchar(128) not null,
LastName varchar(128) not null,
PhoneNumber int not null,
InsurenceNumber varchar(128) not null,
--Atomizace adresy bydliste
City varchar(128) not null,
PostCode varchar(128) not null,
Address varchar(128) not null,
--Objednany pacient
ReceptionistID int,
constraint ReceptionistAppointmentFK
foreign key (ReceptionistID) references Personel (PersonalNumber)
);

create table Departements(
    DepShortcut varchar(128) not null primary key,
    DepName varchar(128) not null,
    FloorNumber int not null,
    NurseID int not null,
    constraint DepNurseFK
    foreign key (NurseID) references Personel (PersonalNumber)
);

create table Rooms(
    RoomNumber int generated as identity not null primary key,
    FloorNumber int not null,
    DepShortcut varchar(128) not null,
    constraint DepartementFK
    foreign key (DepShortcut) references Departements (DepShortcut)
);

create table Medications(
    MedID int generated as identity not null primary key,
    MedName varchar(128) not null,
    ActiveIngredient varchar(128) not null,
    Dosage varchar(128) not null
);

create table Hospitalizations(
    HosID int generated as identity not null primary key,
    --PatientFirstName varchar(128) not null,
    --PatientLastName varchar(128) not null,
    HosDate date not null,
    NationalID char(10) not null,
    constraint HospPatientFK
    foreign key (NationalID) references Patients (NationalID),
    DoctorID int not null,
    constraint HospDoctorFK
    foreign key (DoctorID) references Personel (PersonalNumber)
);

create table DoctorMedication(
    DocMedID int generated as identity not null primary key,
    DoctorID int not null,
    MedID int not null,
    constraint DocMedDoctorFK
    foreign key (DoctorID) references Personel (PersonalNumber),
    constraint DocMedMedicationFK
    foreign key (MedID) references Medications (MedID)
);

create table HospitalizationRoom(
    HosID int not null,
    RoomNumber int not null,
    constraint HosRoomHospitalizationFK
    foreign key (HosID) references Hospitalizations (HosID),
    constraint HosRoomRoomFK
    foreign key (RoomNumber) references Rooms (RoomNumber),
    constraint HosRoomID primary key(HosID, RoomNumber)
);

create table PatientMedication(
    PatientMedID int generated as identity not null primary key,
    NationalID char(10) not null,
    MedID int not null,
    constraint PatientMedPatientFK
    foreign key (NationalID) references Patients (NationalID),
    constraint PatientMedMedicationFK
    foreign key (MedID) references Medications (MedID)
);

create table DoctorDepartement(
    DoctorID int not null,
    DepShortcut varchar(128) not null,
    constraint DoctorDepDoctorFK
    foreign key (DoctorID) references Personel (PersonalNumber),
    constraint DoctorDepDepartementFK
    foreign key (DepShortcut) references Departements (DepShortcut),
    constraint DoctorDepartementID primary key(DoctorID, DepShortcut)
);

insert into Medications (MedName, ActiveIngredient, Dosage)
values ('Ibalgin', 'Dopamine', '15mg');
insert into Medications (MedName, ActiveIngredient, Dosage)
values ('Paralen', 'DMT', '10mg');

insert into Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Pepa', 'Franta', '777666555', '12.12.1975', 'Brno', '66220', 'Rosická 02', 'pepa.franta@gmail.com');
insert into Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Pepakos', 'Frantiskos', '123321123', '1.1.1990', 'Polièka', '61100', 'Líšeòská 01', 'frantiskos_420@seznam.cz');
insert into Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Jan', 'Josefos', '111333222', '2.3.1980', 'Ostrava', '60022', 'Pražská 05', 'xxjosefos@email.com');
insert into Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Janos', 'Panos', '420420420', '13.12.1996', 'Poruba', '60022', 'Havirovska 15', 'JanosPankos@email.com');

insert into Doctors (PersonalNumber, Specialization)
values ('1', 'Chiropraktik');
insert into Doctors (PersonalNumber, Specialization)
values ('4', 'Zubar');

insert into Nurses (PersonalNumber, Specialization)
values ('3', 'Dentalni Hygiena');

insert into Receptionists (PersonalNumber)
values ('2');

insert into Patients (NationalID, FirstName, LastName, PhoneNumber, InsurenceNumber, City, PostCode, Address, receptionistid)
values ('0104174114', 'Pavel', 'Hermann', '+420123456789', '211', 'Brno', '61200', 'Bozetechova 2', '2');
insert into Patients (NationalID, FirstName, LastName, PhoneNumber, InsurenceNumber, City, PostCode, Address)
values ('3322114114', 'Jan', 'Han', '712222333', '201', 'Brno', '62222', 'Bozetechova 8');
insert into Patients (NationalID, FirstName, LastName, PhoneNumber, InsurenceNumber, City, PostCode, Address)
values ('1122332112', 'Alex', 'Josef', '+420123456777', '211', 'Praha', '68800', 'Kolbenova 12');
insert into Patients (NationalID, FirstName, LastName, PhoneNumber, InsurenceNumber, City, PostCode, Address)
values ('3322112112', 'Alexos', 'Kaves', '777666555', '211', 'Ostrava', '60022', 'Ostravska 34');

insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('DA', 'Dìtská Ambulance', '1', '1');
insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('LOG', 'Logopedie', '2', '1');
insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('ORL', 'Ušní, nosní, krèní', '1', '1');
insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('ONK', 'Onkologie', '4', '1');
insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('CHIR', 'Chirurgie', '0', '1');

insert into Rooms (FloorNumber, DepShortcut)
values ('3', 'ONK');
insert into Rooms (FloorNumber, DepShortcut)
values ('5', 'ONK');
insert into Rooms (FloorNumber, DepShortcut)
values ('2', 'CHIR');
insert into Rooms (FloorNumber, DepShortcut)
values ('4', 'LOG');

insert into Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('01-03-2021', 'DD-MM-YYYY'), '0104174114', '1');
insert into Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('01-03-2020', 'DD-MM-YYYY'), '0104174114', '1');
insert into Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('02-03-2020', 'DD-MM-YYYY'), '3322114114', '1');
insert into Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('13-12-2020', 'DD-MM-YYYY'), '0104174114', '2');
insert into Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('12-10-2021', 'DD-MM-YYYY'), '3322114114', '1');
insert into Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('01-04-2021', 'DD-MM-YYYY'), '3322114114', '1');

insert into DoctorMedication(DoctorID, MedID)
values('1', '2');

insert into HospitalizationRoom(HosID, RoomNumber)
values('1', '3');

insert into PatientMedication(NationalID, MedID)
values('3322114114', '2');
insert into PatientMedication(NationalID, MedID)
values('3322114114', '1');
insert into PatientMedication(NationalID, MedID)
values('0104174114', '1');
insert into PatientMedication(NationalID, MedID)
values('3322112112', '2');

insert into DoctorDepartement(DoctorID, DepShortcut)
values('1', 'DA');


--DVA DOTAZY:   spojeni dvou tabulek
--JEDEN DOTAZ:  spojeni tri tabulek
--DVA DOTAY:    vyuzivajici group by + agregacni funkce
--JEDEN DOTAZ:  obsahujici predikat exists
--JEDEN DOTAZ:  s predikatem IN (vnoreny select)

--DVA DOTAZY:   spojeni dvou tabulek
--Ktery pacient byl hospitalizovan dne 1.4.2021
SELECT * FROM Hospitalizations NATURAL JOIN Patients
    WHERE '01-04-2021' = TO_CHAR(hosdate, 'DD-MM-YYYY');
    
--Jake je specializace sestry pracujici na detske ambulanci
SELECT Specialization FROM nurses NATURAL JOIN departements
    WHERE DepShortcut = 'DA';

--JEDEN DOTAZ:  spojeni tri tabulek
--KTERY LEK BYL PREDEPSAN PACIENTU JANU HANOVI
SELECT medname FROM Patients NATURAL JOIN Patientmedication NATURAL JOIN Medications
    WHERE firstname = 'Jan' AND lastname = 'Han' GROUP BY medname;

--DVA DOTAY: vyuzivajici group by + agregacni funkce
--Kolikrat byl pacient Pavel Hermann hospitalizovan v jednotlivych letech
SELECT count(*), TO_CHAR(HosDate, 'YYYY') FROM Patients NATURAL JOIN Hospitalizations
    WHERE FirstName = 'Pavel' AND LastName = 'Hermann' GROUP BY TO_CHAR(HosDate, 'YYYY');
    
--Ktery doktor provedl v roce 2020 nejvice hospitalizaci a kolik jich bylo
SELECT * FROM (SELECT firstname, lastname, count(*) pocet
    FROM Personel NATURAL JOIN Doctors NATURAL JOIN Hospitalizations
    WHERE '2020' = TO_CHAR(HosDate, 'yyyy')
    GROUP BY firstname, lastname ORDER BY pocet DESC) WHERE rownum = 1;

--JEDEN DOTAZ:  obsahujici predikat exists
--Vypis pouze pacienty co nemaji predepsany paralen
SELECT FirstName, LastName FROM Patients
    WHERE NOT EXISTS(SELECT * FROM PatientMedication NATURAL JOIN Medications
    WHERE MedName = 'Paralen' AND NationalID = Patients.NationalID); 

--JEDEN DOTAZ:  s predikatem IN (vnoreny select)
--Vypis pacienty kteri nemaji stejne krestni jmeno jako kdokoliv z personalu nemocnice
SELECT * FROM Patients WHERE FirstName NOT IN(SELECT FirstName FROM Personel);
