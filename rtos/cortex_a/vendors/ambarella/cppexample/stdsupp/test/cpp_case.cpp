/**
 *  @file cpp_case.cpp
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Test code for C++
 */

#include <iostream>
/* Standard Template Library (STL) */
#include <vector>
#include <algorithm>

extern "C" {
#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
}

//#pragma GCC optimize ("O0")

using namespace std;

#include <string>

//#define AmbaKAL_TaskSleep(a) ((void)a)
#if 0
#define printf(pFmt, ...) { \
    char local_printbuf[256]; \
    snprintf(local_printbuf, 256, pFmt, ##__VA_ARGS__); \
    AmbaPrint_PrintStr5("%s",local_printbuf,NULL,NULL,NULL,NULL); \
    AmbaPrint_Flush(); \
}
#endif

void function1(void)
{
    printf("string var using test 1  \n");
    AmbaKAL_TaskSleep(1000);
    int testType;

    string testTypeStr = "TEST"; // ---------------- memcpy() will crush here
    printf("string var using test 2  \n");
    AmbaKAL_TaskSleep(1000);

    if (testTypeStr == "TEST") {
        testType = 0x11;
    } else {
        testType = 0x22;
    }

	printf("testType: 0x%x\n", testType);
    printf("string var using test 3  \n");
}



void function2(unsigned long x)
{
    switch (x) {
    case 0xC0000015:
        char *endptr;

        char b6[] = "9.9500000476837e-001";
        printf("b6=%lf\n", strtod(b6, &endptr)); // ---------------------OK
        AmbaKAL_TaskSleep(1000);

        char b7[] = "9.95000004768371e-001";
        printf("b7=%lf\n", strtod(b7, &endptr)); // ---------------------OK
        AmbaKAL_TaskSleep(1000);

        char b8[] = "9.950000047683715e-001";
        printf("b8=%lf\n", strtod(b8, &endptr)); // ---------------- memcpy() will crush here
        AmbaKAL_TaskSleep(1000);
//        char c[] = "9.9500000476837158e-001";
        printf("test here1\n");
        AmbaKAL_TaskSleep(1000);
        break;
    }
}



class myClass
{
    public:
        myClass(int);
        double tst_exception(int a, int b);

    private:
        int i;
};

myClass::myClass(int x) : i(x) {}

double myClass::tst_exception(int a, int b)
{
    if (b == 0) {
#ifdef __EXCEPTIONS
        throw 13;
#else
        return 0;
#endif
    }

    return (a / b);
}

void test_stl_vector(void)
{
    int myArray[] = { 30, 12, 55, 31, 98, 11, 41, 80, 66, 21 };

    vector<int> myV(myArray, myArray + 10);


    printf("Push 17\r\n");
    myV.push_back(17);

    printf("Size: %d\r\n", myV.size());

    // Sort
    printf("Sort\r\n");
    sort(myV.begin(), myV.end());

    for (vector<int>::iterator it = myV.begin();
         it != myV.end();
         ++it) {
        printf("%d ", *it);
    }
    printf("\r\n");

    int search = 31;

    vector<int>::iterator it =
        find(myV.begin(), myV.end(), search);

    printf("Find %d...\r\n", search);
    if (it != myV.end()) {
        printf("Got it!\r\n");
    } else {
        printf("Can not find!\r\n");
    }

    printf("Pop last\r\n");
    myV.pop_back();

    // Reverse
    printf("Reverse sort\r\n");
    reverse(myV.begin(), myV.end());

    for (vector<int>::iterator it = myV.begin(); it != myV.end();
         ++it) {
        printf("%d ", *it);
    }
    printf("\r\n");
}

extern int Opencv_CannyTest(void);

extern "C" void cpp_test(void);

void cpp_test(void)
{
    printf("\r\n%s ....\r\n", __func__);

    function1();
    function2(0xC0000015);

    myClass my_class(5);

    // dynamic
    myClass* my_class_ptr = new myClass(5);
    double d;

    __try {
        d = my_class_ptr->tst_exception(2, 0);
        printf("d: %lf\n",d);
#ifdef __EXCEPTIONS
    } catch (int c) {
        printf("Exception int: %d ...\r\n", c);
    } catch (std::exception& e) {
        printf("Exception std ...\r\n");
#endif
    }

    delete my_class_ptr;

    // Test STL
    test_stl_vector();

#ifdef CONFIG_BUILD_FOSS_OPENCV
    int ret;
    ret = Opencv_CannyTest();
    printf("Opencv_CannyTest() returns %d\n",ret);
#endif
}



/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

void *__dso_handle = NULL;

#ifdef __cplusplus
}
#endif

