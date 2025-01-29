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
drop sequence personal_number;

create table Personel(
PersonalNumber int not null primary key,
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
--R? má maximáln? 10 znak? bez lomítka a je d?litelné 11
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

/*--- Display values of variables ---*/
set serveroutput on;

/*--- Trigger for generating personal number ---*/
CREATE SEQUENCE personal_number increment by 1 start with 1;
CREATE OR REPLACE TRIGGER personal_number_gen BEFORE INSERT ON personel FOR EACH ROW
begin
    :new.personalnumber := personal_number.nextval;
end;
/

/*--- Trigger for assigning patiens to doctors evently by specialization ---*/
CREATE OR REPLACE TRIGGER distribute_evenly 
    BEFORE INSERT OR UPDATE ON Hospitalizations FOR EACH ROW
DECLARE
    doctor_id number; --certain doctor
    free_doctors_COUNTer number := 0; --number of doctors that have no patient
    spec varchar(30);
BEGIN
    
    SELECT specialization INTO spec FROM doctors WHERE doctorid = :new.doctorid;

    SELECT COUNT(doctorid) INTO free_doctors_COUNTer FROM doctors 
        WHERE not exists (SELECT doctorid FROM hospitalizations 
            WHERE doctors.doctorid = hospitalizations.doctorid)
                AND doctors.specialization = spec;

    if free_doctors_COUNTer = 0 then
    SELECT doctorid INTO doctor_id FROM (SELECT doctorid, COUNT(*) pocet
            FROM Personel NATURAL JOIN Doctors NATURAL JOIN Hospitalizations 
                WHERE doctors.specialization = spec
                GROUP BY doctorid ORDER BY pocet ASC) WHERE rownum=1;
        :new.DoctorID := doctor_id;
    else
    SELECT doctorid INTO doctor_id FROM doctors WHERE not exists(SELECT doctorid FROM hospitalizations
            WHERE doctors.doctorid = hospitalizations.doctorid) AND doctors.specialization = spec AND rownum = '1';
            :new.DoctorID := doctor_id;
    end if;
END;
/

/*--- Display all patiens, that use given medication and were hospitalized by doctor id ---*/
CREATE OR REPLACE PROCEDURE who_has_medicament(medicament_id int, doctor_id int)
AS
    CURSOR medicines IS
    SELECT * FROM patients NATURAL JOIN patientmedication NATURAL JOIN hospitalizations
        where medid = medicament_id AND doctorid = doctor_id;
    CURSOR_row medicines%ROWTYPE;
BEGIN
    OPEN medicines;
    LOOP
        FETCH medicines INTO CURSOR_row;
        EXIT WHEN medicines%notfound;
        DBMS_OUTPUT.put_line(
            'Firstname: ' || CURSOR_row.FirstName || chr(10) ||
            'Lastname: ' || CURSOR_row.LastName || chr(10) ||
            'MedicineID: ' || CURSOR_row.medid || chr(10) ||
            'DoctorID: ' || CURSOR_row.doctorid || chr(10));
    END LOOP;
    CLOSE medicines;
    EXCEPTION WHEN OTHERS THEN
        DBMS_OUTPUT.put_line('Unknown medicine or Doctor');
END;
/

/*--- Doctors stats ---*/
CREATE OR REPLACE PROCEDURE doctor_prescribed_stats
AS
    CURSOR doctors IS
    SELECT * FROM doctors;
    CURSOR_row doctors%ROWTYPE;
    number_of_prescripted_meds int;
    first_name varchar(20);
    last_name varchar(20);
BEGIN
    OPEN doctors;
    LOOP
        FETCH doctors INTO CURSOR_row;
        EXIT WHEN doctors%notfound;
        
        SELECT firstname, lastname INTO first_name, last_name FROM doctors
            NATURAL JOIN personel WHERE doctorid = CURSOR_row.doctorid;
            
        SELECT COUNT(*) INTO number_of_prescripted_meds FROM doctormedication
            NATURAL JOIN doctors WHERE doctorid = CURSOR_row.doctorid;
        
        --Display statistics of each Doctor    
        DBMS_OUTPUT.put_line(
        'Firstname: ' || first_name || chr(10) ||
        'Lastname: ' || last_name || chr(10) ||
        'Number of prescribed medicines: ' || to_char(number_of_prescripted_meds) || chr(10));
    END LOOP;
    CLOSE doctors;
END;
/

/*--- Inserting values into tables ---*/
insert INTO Medications (MedName, ActiveIngredient, Dosage)
values ('Ibalgin', 'Dopamine', '15mg');
insert INTO Medications (MedName, ActiveIngredient, Dosage)
values ('Paralen', 'DMT', '10mg');
insert INTO Medications (MedName, ActiveIngredient, Dosage)
values ('Robitusin', 'Codein', '4mg');

insert INTO Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Pepa', 'Franta', '777666555', '12.12.1975', 'Brno', '66220', 'Rosická 02', 'pepakos95@gmail.com');
insert INTO Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Pepakos', 'Frantiskos', '123321123', '1.1.1990', 'Polièka', '61100', 'Líšeòská 01', 'frantiskos123@seznam.cz');
insert INTO Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Jan', 'Josefos', '111333222', '2.3.1980', 'Ostrava', '60022', 'Pražská 05', 'xxjosefos@email.com');
insert INTO Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Janos', 'Panos', '789654265', '13.12.1996', 'Poruba', '60022', 'Havirovska 15', 'JanosPankos@email.com');
insert INTO Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Pavel', 'Jánek', '777658987', '13.12.1993', 'Plzeò', '60022', 'Božetìchova 11', 'pafka3@email.com');
insert INTO Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Alexander', 'Sila', '123456789', '13.12.1992', 'Svitavy', '60022', 'Námìstí Míru 1', 'aleks.sila@email.com');
insert INTO Personel (FirstName, LastName, PhoneNumber, BirthDate, City, PostCode, Address, Email)
values ('Jožo', 'Mendel', '777569841', '13.12.1991', 'Zlín', '60022', 'Veveøí 32', 'jozo.mendel@email.com');

insert INTO Doctors (PersonalNumber, Specialization)
values ('1', 'Chiropraktik');
insert INTO Doctors (PersonalNumber, Specialization)
values ('4', 'Zubar');
insert INTO Doctors (PersonalNumber, Specialization)
values ('5', 'Chiropraktik');
insert INTO Doctors (PersonalNumber, Specialization)
values ('6', 'Zubar');
insert INTO Doctors (PersonalNumber, Specialization)
values ('7', 'Urolog');

insert INTO Nurses (PersonalNumber, Specialization)
values ('3', 'Dentalni Hygiena');

insert INTO Receptionists (PersonalNumber)
values ('2');

insert INTO Patients (NationalID, FirstName, LastName, PhoneNumber, InsurenceNumber, City, PostCode, Address, receptionistid)
values ('0104174114', 'Pavel', 'Hermann', '+420123456789', '211', 'Brno', '61200', 'Bozetechova 2', '2');
insert INTO Patients (NationalID, FirstName, LastName, PhoneNumber, InsurenceNumber, City, PostCode, Address, receptionistid)
values ('3322114114', 'Jan', 'Han', '712222333', '201', 'Brno', '62222', 'Bozetechova 8', '2');
insert INTO Patients (NationalID, FirstName, LastName, PhoneNumber, InsurenceNumber, City, PostCode, Address, receptionistid)
values ('1122332112', 'Alex', 'Josef', '+420123456777', '211', 'Praha', '68800', 'Kolbenova 12', '2');
insert INTO Patients (NationalID, FirstName, LastName, PhoneNumber, InsurenceNumber, City, PostCode, Address, receptionistid)
values ('3322112112', 'Marie', 'Neeko', '777666555', '211', 'Ostrava', '60022', 'Ostravska 34', '2');

insert INTO Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('DA', 'Dìtská Ambulance', '1', '1');
insert INTO Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('LOG', 'Logopedie', '2', '1');
insert INTO Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('ORL', 'Ušní, nosní, krèní', '1', '1');
insert INTO Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('ONK', 'Onkologie', '4', '1');
insert INTO Departements (DepShortcut, DepName, FloorNumber, NurseID)
values ('CHIR', 'Chirurgie', '0', '1');

insert INTO Rooms (FloorNumber, DepShortcut)
values ('3', 'ONK');
insert INTO Rooms (FloorNumber, DepShortcut)
values ('5', 'ONK');
insert INTO Rooms (FloorNumber, DepShortcut)
values ('2', 'CHIR');
insert INTO Rooms (FloorNumber, DepShortcut)
values ('4', 'LOG');

insert INTO Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('02-03-2020', 'DD-MM-YYYY'), '3322114114', '3');
insert INTO Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('13-12-2020', 'DD-MM-YYYY'), '0104174114', '3');
insert INTO Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('12-10-2021', 'DD-MM-YYYY'), '3322114114', '1');
insert INTO Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('01-04-2021', 'DD-MM-YYYY'), '3322114114', '2');
insert INTO Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('01-04-2021', 'DD-MM-YYYY'), '3322114114', '1');

insert INTO DoctorMedication(DoctorID, MedID)
values('1', '2');
insert INTO DoctorMedication(DoctorID, MedID)
values('2', '1');
insert INTO DoctorMedication(DoctorID, MedID)
values('2', '1');

insert INTO HospitalizationRoom(HosID, RoomNumber)
values('1', '3');

insert INTO PatientMedication(NationalID, MedID)
values('3322114114', '2');
insert INTO PatientMedication(NationalID, MedID)
values('3322114114', '1');
insert INTO PatientMedication(NationalID, MedID)
values('0104174114', '1');

insert INTO DoctorDepartement(DoctorID, DepShortcut)
values('1', 'DA');


------GRANT PERMISSION-----
GRANT ALL ON Departements TO XSILAA00;
GRANT ALL ON Doctordepartement TO XSILAA00;
GRANT ALL ON Doctormedication TO XSILAA00;
GRANT ALL ON Doctors TO XSILAA00;
GRANT ALL ON Hospitalizationroom TO XSILAA00;
GRANT ALL ON Hospitalizations TO XSILAA00;
GRANT ALL ON Medications TO XSILAA00;
GRANT ALL ON Nurses TO XSILAA00;
GRANT ALL ON Patientmedication TO XSILAA00;
GRANT ALL ON Patients TO XSILAA00;
GRANT ALL ON Personel TO XSILAA00;
GRANT ALL ON Receptionists TO XSILAA00;
GRANT ALL ON Rooms TO XSILAA00;

/*TRIGGER DEMONSTRATION*/
--If a hospitalization is added this trigger will distribute patients between -
--doctors evenly(based on doctors specialization)
/*
  insert INTO Hospitalizations(HosDate, NationalID, DoctorID)
    values (TO_DATE('01-03-2021', 'DD-MM-YYYY'), '0104174114', '4');
  insert INTO Hospitalizations(HosDate, NationalID, DoctorID)
    values (TO_DATE('01-03-2020', 'DD-MM-YYYY'), '0104174114', '4');
*/
--Doctor with ID = 4 has just one hospitalization and Doctor with ID = 2 
--has the other
--SELECT * FROM hospitalizations;

/* --- Procedure --- */
--Print all information about patients, that are using a specific 'medicament id' AND were hospitalized by certain doctor

insert INTO PatientMedication(NationalID, MedID)
values('3322112112', '3');
insert INTO Hospitalizations(HosDate, NationalID, DoctorID)
values (TO_DATE('01-04-2021', 'DD-MM-YYYY'), '3322112112', '5');
CALL who_has_medicament('3','5');


/* --- Procedure --- */
--Print doctor statistics
--Doctor with ID = 5 prescribed one medicine
insert INTO DoctorMedication(DoctorID, MedID)
values('5', '3');
CALL doctor_prescribed_stats();


/*--- CREATE MATARIALIZED VIEW ---*/
DROP MATERIALIZED VIEW Doctor_most_hosp_in_year1;

CREATE MATERIALIZED VIEW Doctor_most_hosp_in_year1 AS
SELECT * FROM (SELECT firstname, lastname, COUNT(*) pocet
    FROM xsilaa00.Personel NATURAL JOIN xsilaa00.Doctors NATURAL JOIN xsilaa00.Hospitalizations
    WHERE '2020' = TO_CHAR(HosDate, 'yyyy')
    GROUP BY firstname, lastname ORDER BY pocet DESC) WHERE rownum = 1;


/*--- WITHOUT INDEXES ---*/
EXPLAIN PLAN FOR
--Number of times patient Pavel Hermann has been hospitalized in each year
SELECT COUNT(*), TO_CHAR(HosDate, 'YYYY') FROM Patients NATURAL JOIN Hospitalizations
    WHERE FirstName = 'Pavel' AND LastName = 'Hermann' GROUP BY TO_CHAR(HosDate, 'YYYY');

SELECT PLAN_TABLE_OUTPUT FROM table (DBMS_XPLAN.DISPLAY());


/*--- Indexes for optimalization ---*/
CREATE INDEX pacients_index on patients(nationalid, firstname, lastname);
CREATE INDEX dates_index on hospitalizations(nationalid,hosdate);


/*--- WITH INDEXES ---*/
EXPLAIN PLAN FOR
--Number of times patient Pavel Hermann has been hospitalized in each year
SELECT COUNT(*), TO_CHAR(HosDate, 'YYYY') FROM Patients NATURAL JOIN Hospitalizations
    WHERE FirstName = 'Pavel' AND LastName = 'Hermann' GROUP BY TO_CHAR(HosDate, 'YYYY');

SELECT PLAN_TABLE_OUTPUT FROM table (DBMS_XPLAN.DISPLAY());


