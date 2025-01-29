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
    PersonalNumber int not null,
    constraint ReceptionistFK
    foreign key (PersonalNumber) references Personel(PersonalNumber)
);

create table Doctors(
    PersonalNumber int not null,
    constraint DoctorFK
    foreign key (PersonalNumber) references Personel(PersonalNumber),
    Specialization varchar(128) not null
);

create table Nurses(
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
    DepartShortcut varchar(128) not null,
    constraint DepartementFK
    foreign key (DepartShortcut) references Departements (DepShortcut)
);

create table Medications(
    MedID int generated as identity not null primary key,
    MedName varchar(128) not null,
    ActiveIngredient varchar(128) not null,
    Dosage varchar(128) not null
);

create table Hospitalizations(
    HosID int generated as identity not null primary key,
    PatientFirstName varchar(128) not null,
    PatientLastName varchar(128) not null,
    HosDate date not null,
    PatientID char(10) not null,
    constraint HospPatientFK
    foreign key (PatientID) references Patients (NationalID),
    DoctorID int not null,
    constraint HospDoctorFK
    foreign key (DoctorID) references Personel (PersonalNumber)
);

create table DoctorMedication(
    DoctorID int not null,
    MedicationID int not null,
    constraint DocMedDoctorFK
    foreign key (DoctorID) references Personel (PersonalNumber),
    constraint DocMedMedicationFK
    foreign key (MedicationID) references Medications (MedID),
    constraint DocMedID primary key (DoctorID, MedicationID)
);

create table HospitalizationRoom(
    HospitalizationID int not null,
    RoomID int not null,
    constraint HosRoomHospitalizationFK
    foreign key (HospitalizationID) references Hospitalizations (HosID),
    constraint HosRoomRoomFK
    foreign key (RoomID) references Rooms (RoomNumber),
    constraint HosRoomID primary key(HospitalizationID, RoomID)
);

create table PatientMedication(
    PatientID char(10) not null,
    MedicationID int not null,
    constraint PatientMedPatientFK
    foreign key (PatientID) references Patients (NationalID),
    constraint PatientMedMedicationFK
    foreign key (MedicationID) references Medications (MedID),
    constraint PatientMedicationID primary key(PatientID, MedicationID)
);

create table DoctorDepartement(
    DoctorID int not null,
    DepartementShortcut varchar(128) not null,
    constraint DoctorDepDoctorFK
    foreign key (DoctorID) references Personel (PersonalNumber),
    constraint DoctorDepDepartementFK
    foreign key (DepartementShortcut) references Departements (DepShortcut),
    constraint DoctorDepartementID primary key(DoctorID, DepartementShortcut)
);

insert into Medications (MedName, ActiveIngredient, Dosage)
values ('Ibalgin', 'Dopamine', '15mg');
insert into Medications (MedName, ActiveIngredient, Dosage)
values ('Paralen', 'DMT', '10mg');

insert into Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Pepa', 'Franta', '777666555', '12.12.1975', 'Pelhøimov', '66220', 'Rosická 02', 'pepa.franta@gmail.com');
insert into Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Pepakos', 'Frantiskos', '123321123', '1.1.1990', 'Polièka', '61100', 'Líšeòská 01', 'frantiskos_420@seznam.cz');
insert into Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Frantiskos', 'Josefos', '111333222', '2.3.1980', 'Ostrava', '60022', 'Pražská 05', 'xxjosefos@email.com');

insert into Doctors (PersonalNumber, Specialization)
values ('1', 'Chiropraktik');

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
values ('DA', 'Dìtská Ambulance', '1', '3');
insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('LOG', 'Logopedie', '2', '3');
insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('ORL', 'Ušní, nosní, krèní', '1', '3');
insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('ONK', 'Onkologie', '4', '3');
insert into Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('CHIR', 'Chirurgie', '0', '3');

insert into Rooms (FloorNumber, DepartShortcut)
values ('3', 'ONK');
insert into Rooms (FloorNumber, DepartShortcut)
values ('5', 'ONK');
insert into Rooms (FloorNumber, DepartShortcut)
values ('2', 'CHIR');
insert into Rooms (FloorNumber, DepartShortcut)
values ('4', 'LOG');

insert into Hospitalizations(PatientFirstName, PatientLastName, HosDate, PatientID, DoctorID)
values ('Pavel', 'Hermann', TO_DATE('01-04-2021', 'DD-MM-YYYY'), '0104174114', '1');
insert into Hospitalizations(PatientFirstName, PatientLastName, HosDate, PatientID, DoctorID)
values ('Jan', 'Han', TO_DATE('12-10-2021', 'DD-MM-YYYY'), '3322114114', '1');

insert into DoctorMedication(DoctorID, MedicationID)
values('1', '2');

insert into HospitalizationRoom(HospitalizationID, RoomID)
values('1', '3');

insert into PatientMedication(PatientID, MedicationID)
values('0104174114', '2');

insert into DoctorDepartement(DoctorID, DepartementShortcut)
values('1', 'DA');

