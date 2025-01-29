//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Pavel Heřmann <xherma34@stud.fit.vutbr.cz>
// $Date:       $2021-03-05
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Pavel Heřmann
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"
#include <vector>

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//

class TestMatrix:public::testing::Test
{
    protected:
        Matrix matrix();
};

TEST_F(TestMatrix, Constructor)
{
    //Construct spravne zadanych matic
    EXPECT_NO_THROW(Matrix());
    EXPECT_NO_THROW(Matrix(1, 1));
    EXPECT_NO_THROW(Matrix(1, 8));
    EXPECT_NO_THROW(Matrix(8, 1));
    EXPECT_NO_THROW(Matrix(4, 4));

    //Construct spatne zadanych matic
    EXPECT_ANY_THROW(Matrix(0, 0));
    EXPECT_ANY_THROW(Matrix(-1, 10));
    EXPECT_ANY_THROW(Matrix(10, -1));
}

TEST_F(TestMatrix, SetSingleValue)
{
    //Spravne nastaveni hodnoty matice 1x1
    Matrix matrix1 = Matrix();
    EXPECT_TRUE(matrix1.set(0, 0, 1));
    EXPECT_TRUE(matrix1.set(0, 0, 150000));
    EXPECT_TRUE(matrix1.set(0, 0, -20));

    //Spatne nastaveni hodnoty 1x1
    EXPECT_FALSE(matrix1.set(1, 0, 10));
    EXPECT_FALSE(matrix1.set(0, 1, 10));

    //Spravne nastaveni ctvercove matice > 1x1
    Matrix matrix2 = Matrix(4, 4);
    EXPECT_TRUE(matrix2.set(2, 1, 1));
    EXPECT_TRUE(matrix2.set(1, 3, 150000));
    EXPECT_TRUE(matrix2.set(3, 3, -20));

    //Spatne nastaveni ctvercove matice > 1x1
    EXPECT_FALSE(matrix2.set(-1, 2, 10));
    EXPECT_FALSE(matrix2.set(30, 1, 10));
}

TEST_F(TestMatrix, SetArrayOfValues)
{
    //Nastaveni matice 1x1
    Matrix matrix1 = Matrix();
    EXPECT_TRUE(matrix1.set(std::vector<std::vector<double>> {{1}}));
    EXPECT_TRUE(matrix1.set(std::vector<std::vector<double>> {{-505.505}}));
    EXPECT_FALSE(matrix1.set(std::vector<std::vector<double>> {{1, 2, 3, 1, 2, 3, 1, 2, 3}}));

    //Ctvercova matice 4x4
    Matrix matrix4x4 = Matrix(4, 4);
    EXPECT_TRUE(matrix4x4.set(std::vector<std::vector<double>> {{1, 2, -3, 8}, {1, 13, -3, 8}, {18, 6, 3, 8}, {8, 6, 3, 8}}));
    EXPECT_TRUE(matrix4x4.set(std::vector<std::vector<double>> {{100, 0.505, -308.308, 123578.215}, {100, 0.505, -308.308, 123578.215}, 
    {100, 0.505, -308.308, 123578.215}, {100, 0.505, -308.308, 123578.215}}));
    //spatne nastaveni matice 4x4
    EXPECT_FALSE(matrix4x4.set(std::vector<std::vector<double>> {{1, 2, 3, 1, 2, 3, 1, 2, 3}}));
    EXPECT_FALSE(matrix4x4.set(std::vector<std::vector<double>> {{0}}));

    //Nastaveni matixe 3x3
    Matrix matrix3x3 = Matrix(3, 3);
    EXPECT_TRUE(matrix3x3.set(std::vector<std::vector<double>> {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}));
    EXPECT_TRUE(matrix3x3.set(std::vector<std::vector<double>> {{-100, 0.505, -500.505}, {-100, 0.505, -500.505}, {-100, 0.505, -500.505}}));
    //Spatne nastaveni matice 3x3
    EXPECT_FALSE(matrix3x3.set(std::vector<std::vector<double>> {{1, 2, 3, 1, 2, 3, 1, 2, 3}}));
    EXPECT_FALSE(matrix3x3.set(std::vector<std::vector<double>> {{0}}));

    //nastaveni nectvercove matice
    Matrix matrixA = Matrix(2, 3);
    EXPECT_TRUE(matrixA.set(std::vector<std::vector<double>> {{1, 2, 3}, {1, 2, 3}}));
    EXPECT_TRUE(matrixA.set(std::vector<std::vector<double>> {{-100, 0.505, -500.505}, {-100, 0.505, -500.505}}));
    //Spatne nastaveni nectvercove matice
    EXPECT_FALSE(matrixA.set(std::vector<std::vector<double>> {{1, 2, 3, 1, 2, 3, 1, 2, 3}}));
    EXPECT_FALSE(matrixA.set(std::vector<std::vector<double>> {{0}}));
}

TEST_F(TestMatrix, Get)
{
    //Testovani nastavene hodnoty pro ctvercovou matici
    Matrix matrix3x3 = Matrix(3, 3);
    matrix3x3.set(2, 2, 10);
    matrix3x3.set(0, 1, -500.505);
    matrix3x3.set(1, 0, 0);
    EXPECT_EQ(matrix3x3.get(2, 2), 10);
    EXPECT_EQ(matrix3x3.get(0, 1), -500.505);
    EXPECT_EQ(matrix3x3.get(1, 0), 0);
    //Testovani spatneho indexu ctvercove matice
    EXPECT_ANY_THROW(matrix3x3.get(-10, 2));
    EXPECT_ANY_THROW(matrix3x3.get(2, -10));  

    //Testovani nastavene hodnotz pro matici 1x1
    Matrix matrix1 = Matrix();
    matrix1.set(0, 0, 5);
    EXPECT_EQ(matrix1.get(0, 0), 5);
    //Spatne indexovani 1x1
    EXPECT_ANY_THROW(matrix1.get(1, 8));
    EXPECT_ANY_THROW(matrix1.get(8, 1));
    EXPECT_ANY_THROW(matrix1.get(-5, 0));
    EXPECT_ANY_THROW(matrix1.get(0, -5));
}

TEST_F(TestMatrix, MatrixEquals)
{
    //Porovnani 2 ctvercovych matic
    Matrix matrix1 = Matrix(3, 3);
    Matrix matrix2 = Matrix(3, 3);
    matrix1.set(2, 2, 10);
    matrix2.set(2, 2, 10);
    //Porovnani identickych ctvercovych matic
    EXPECT_NO_THROW(matrix1 == matrix2);
    EXPECT_TRUE(matrix1 == matrix2);
    //Vyruseni rovnosti matic
    matrix2.set(0, 0, 500);
    EXPECT_FALSE(matrix1 == matrix2);

    //Porovnani matic ruznych velikosti
    Matrix matrix3 = Matrix(2, 3);
    Matrix matrix4 = Matrix(3, 2);
    EXPECT_ANY_THROW(matrix3 == matrix4);

}

TEST_F(TestMatrix, MatrixAdd)
{
    //Scitani ctvercove matice 2x2
    Matrix matrix1 = Matrix(2, 2);
    Matrix matrix2 = Matrix(2, 2);
    Matrix result = Matrix(2, 2);
    matrix1.set(std::vector<std::vector<double>> {{1, 1}, {1, 1}});
    matrix2.set(std::vector<std::vector<double>> {{1, 1}, {1, 1}});
    result.set(std::vector<std::vector<double>> {{2, 2}, {2, 2}});
    
    EXPECT_NO_THROW(matrix1+matrix2);
    EXPECT_EQ(result, matrix1+matrix2);
    EXPECT_EQ(result, matrix2+matrix1);

    //scitani matic s ruznou velikosti
    Matrix matrix5 = Matrix(1, 2);
    Matrix matrix6= Matrix(2, 1);
    EXPECT_ANY_THROW(matrix5+matrix6);

    //Ctvercova matice 4x4
    Matrix matrix4x4A = Matrix(4, 4);
    Matrix matrix4x4B = Matrix(4, 4);
    Matrix result4x4 = Matrix(4, 4);
    matrix4x4A.set(std::vector<std::vector<double>> {{1, 2, 3, 4}, {4, 3, 2, 1}, {1, 1, 1, 1}, {0, 0, 0, 0}});
    matrix4x4B.set(std::vector<std::vector<double>> {{1, 1, 1, 1}, {1, 1, 1, 1}, {4, 4, 4, 4}, {2, 2, 2, 2}});
    result4x4.set(std::vector<std::vector<double>> {{2, 3, 4, 5}, {5, 4, 3, 2}, {5, 5, 5, 5}, {2, 2, 2, 2}});
    EXPECT_NO_THROW(matrix4x4A+matrix4x4B);
    EXPECT_EQ(result4x4, matrix4x4A+matrix4x4B);
    EXPECT_EQ(result4x4, matrix4x4B+matrix4x4A);

}

TEST_F(TestMatrix, MatrixesMultiply)
{
    //Nasobeni ctvercove matice
    Matrix matrixA = Matrix(2, 2);
    Matrix matrixB = Matrix(2, 2);
    Matrix resultAxB = Matrix(2, 2);
    Matrix resultBxA = Matrix(2, 2);
    matrixA.set(std::vector<std::vector<double>> {{1, 2}, {3, 4}});
    matrixB.set(std::vector<std::vector<double>> {{0, 1}, {3, 5}});
    resultAxB.set(std::vector<std::vector<double>> {{6, 11}, {12, 23}});
    resultBxA.set(std::vector<std::vector<double>> {{3, 4}, {18, 26}});

    EXPECT_NO_THROW(matrixA*matrixB);
    EXPECT_NO_THROW(matrixB*matrixA);
    EXPECT_EQ(resultAxB, matrixA*matrixB);
    EXPECT_EQ(resultBxA, matrixB*matrixA);
    EXPECT_FALSE((matrixA*matrixB) == (matrixB*matrixA));

    //Validni nasobeni matic s ruznou velikosti
    Matrix matrixC = Matrix(2, 1);
    matrixC.set(std::vector<std::vector<double>> {{1}, {2}});
    EXPECT_NO_THROW(matrixA*matrixC);
    EXPECT_ANY_THROW(matrixC*matrixA); 

    //Nevalidni nasobeni matic s ruznou velikosti
    Matrix matrixD = Matrix(4, 1);
    matrixC.set(std::vector<std::vector<double>> {{1}, {2}, {1}, {2}});
    EXPECT_ANY_THROW(matrixA*matrixD);
    EXPECT_ANY_THROW(matrixD*matrixA);
}

TEST_F(TestMatrix, MatrixMultiplyConst)
{
    //Nasobeni ctvercove matice konstantou
    Matrix matrixA = Matrix(2, 2);
    matrixA.set(std::vector<std::vector<double>> {{1, 2}, {2, 1}});
    Matrix result = Matrix(2, 2);
    result.set(std::vector<std::vector<double>> {{2, 4}, {4, 2}});

    EXPECT_NO_THROW(matrixA*2);
    EXPECT_TRUE((matrixA*2)==result);

    //Nasobeni nectvercove matice konstantou
    Matrix matrix3x2 = Matrix(3, 2);
    matrixA.set(std::vector<std::vector<double>> {{1, 2, 3}, {3, 2, 1}});
    Matrix result3x2 = Matrix(3, 2);
    result.set(std::vector<std::vector<double>> {{2, 4, 6}, {6, 4, 2}});

    EXPECT_NO_THROW(matrix3x2*2);
    EXPECT_EQ((matrix3x2*2), result3x2);
}

TEST_F(TestMatrix, MatrixEquation)
{
    //Ctvercova matice 2x2
    Matrix matrix2x2 = Matrix(2, 2);
    matrix2x2.set(std::vector<std::vector<double>> {{1, 2}, {5, 15}});
    //spravne provedeni
    EXPECT_NO_THROW(matrix2x2.solveEquation(std::vector<double>{5, 0}));
    //Chyba u spatne zadane prave strany
    EXPECT_ANY_THROW(matrix2x2.solveEquation(std::vector<double>{5, 0, 10}));
    EXPECT_ANY_THROW(matrix2x2.solveEquation(std::vector<double>{5}));
    //Overeni spravneho vypoctu
    EXPECT_TRUE(matrix2x2.solveEquation(std::vector<double> {5, 0}) == (std::vector<double> {15, -5}));

    //Singularni matice 2x2
    Matrix matrix2x2S = Matrix(2, 2);
    matrix2x2S.set(std::vector<std::vector<double>> {{1, -2}, {-2, 4}});
    EXPECT_ANY_THROW(matrix2x2S.solveEquation(std::vector<double>{5, 0}));


    //Singularni matice 3x3
    Matrix matrix3x3s = Matrix(3, 3);
    matrix3x3s.set(std::vector<std::vector<double>> {{1, -4, -3}, {1, -5, -3}, {-1, 6, 3}});
    EXPECT_ANY_THROW(matrix3x3s.solveEquation(std::vector<double>{5, 0, 10}));

    //Ctvercova matice 4x4
    Matrix matrix4x4 = Matrix(4, 4);
    matrix4x4.set(std::vector<std::vector<double>> {{1, 2, -3, 8}, {1, 13, -3, 8}, {18, 6, 3, 8}, {8, 6, 3, 8}});
    EXPECT_NO_THROW(matrix4x4.solveEquation(std::vector<double>{5, 0, 10, 15}));
    EXPECT_ANY_THROW(matrix4x4.solveEquation(std::vector<double>{5, 0, 10}));
 

    //Nectvercova matice
    Matrix matrixA = Matrix(4, 2);
    matrixA.set(std::vector<std::vector<double>> {{1, 2}, {3, 4}, {5, 6}, {7, 8}});
    Matrix matrixB = Matrix(2, 3);
    matrixB.set(std::vector<std::vector<double>> {{1, 2, 3,}, {4, 5, 6}});
    EXPECT_ANY_THROW(matrixA.solveEquation(std::vector<double>{2, 2, 2, 2}));
    EXPECT_ANY_THROW(matrixB.solveEquation(std::vector<double>{2, 2}));
    //Nectvercova matice spatne zadana prava strana
    EXPECT_ANY_THROW(matrixA.solveEquation(std::vector<double>{2, 2, 2, 2, 2}));
    EXPECT_ANY_THROW(matrixB.solveEquation(std::vector<double>{2, 2, 2}));

    //Matice 1x1
    Matrix matrix1x1 = Matrix();
    matrix1x1.set(0, 0, 2);
    EXPECT_NO_THROW(matrix1x1.solveEquation(std::vector<double>{2}));
    EXPECT_TRUE(matrix1x1.solveEquation(std::vector<double> {2}) == (std::vector<double> {1}));

}

TEST_F(TestMatrix, InverseMatrix)
{
    //matice 1x1
    Matrix matrix1x1 = Matrix();
    matrix1x1.set(0, 0, 1);
    EXPECT_ANY_THROW(matrix1x1.inverse());

    //matice 2x2
    Matrix matrix2x2 = Matrix(2, 2);
    matrix2x2.set(std::vector<std::vector<double>> {{1, 2}, {4, 4}});
    Matrix matrix2x2Res = Matrix(2, 2);
    matrix2x2Res.set(std::vector<std::vector<double>> {{-1, 0.5}, {1, -0.25}});
    //test probehnuti
    EXPECT_NO_THROW(matrix2x2.inverse());
    //test spravneho vypoctu inverzni matice
    EXPECT_TRUE((matrix2x2.inverse()) == matrix2x2Res);

    //matice 3x3
    Matrix matrix3x3 = Matrix(3, 3);
    matrix3x3.set(std::vector<std::vector<double>> {{1, 2, 3}, {2, 4, 4}, {3, 5, 7}});
    Matrix matrix3x3Res = Matrix(3, 3);
    matrix3x3Res.set(std::vector<std::vector<double>> {{-4, -0.5, 2}, {1, 1, -1}, {1, -0.5, 0}});
    //test probehnuti
    EXPECT_NO_THROW(matrix3x3.inverse());
    //test spravneho vypoctu inverzni matice
    EXPECT_TRUE((matrix3x3.inverse()) == matrix3x3Res);

    //Singularni matice 2x2
    Matrix matrix2x2S = Matrix(2, 2);
    matrix2x2S.set(std::vector<std::vector<double>> {{1, -2}, {-2, 4}});
    EXPECT_ANY_THROW(matrix2x2S.inverse());

    //Singularni matice 3x3
    Matrix matrix3x3S = Matrix(3, 3);
    matrix3x3S.set(std::vector<std::vector<double>> {{1, -4, -3}, {1, -5, -3}, {-1, 6, 3}});
    EXPECT_ANY_THROW(matrix3x3S.inverse());
}

TEST_F(TestMatrix, TransposeMatrix)
{
    //matice 1x1
    Matrix matrix1x1 = Matrix();
    matrix1x1.set(0, 0, 1);
    EXPECT_NO_THROW(matrix1x1.transpose());

    //matice 2x2
    Matrix matrix2x2 = Matrix(2, 2);
    matrix2x2.set(std::vector<std::vector<double>> {{1, 2}, {4, 4}});
    Matrix matrix2x2T = Matrix(2, 2);
    matrix2x2T.set(std::vector<std::vector<double>> {{1, 4}, {2, 4}});
    EXPECT_NO_THROW(matrix2x2.transpose());
    EXPECT_TRUE((matrix2x2.transpose()) == matrix2x2T);

    //matice 3x3
    Matrix matrix3x3 = Matrix(3, 3);
    matrix3x3.set(std::vector<std::vector<double>> {{1, 2, 3}, {2, 4, 4}, {3, 5, 7}});
    Matrix matrix3x3T = Matrix(3, 3);
    matrix3x3T.set(std::vector<std::vector<double>> {{1, 2, 3}, {2, 4, 5}, {3, 4, 7}});
    EXPECT_NO_THROW(matrix3x3.transpose());
    EXPECT_TRUE((matrix3x3.transpose()) == matrix3x3T);

    //Nectvercova matice
    Matrix matrixA = Matrix(2, 1);
    matrixA.set(std::vector<std::vector<double>> {{1}, {2}});
    EXPECT_NO_THROW(matrixA.transpose());
}

/*** Konec souboru white_box_tests.cpp ***/
