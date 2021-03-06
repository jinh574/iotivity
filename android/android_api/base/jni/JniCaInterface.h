//******************************************************************
//
// Copyright 2015 Intel Corporation.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <jni.h>
// THIS FILE IS SUBJECT TO CHANGE DUE TO THE ONGOING DEVELOPMENT OF THE CA FOR ANDROID
// DO NOT REVIEW THIS FILE

/* Header for class org_iotivity_ca_CaInterface */

#ifndef _Included_org_iotivity_ca_CaInterface
#define _Included_org_iotivity_ca_CaInterface
#ifdef __cplusplus
extern "C" {
#endif

    void CAManagerConnectionStateChangedCB(const CAEndpoint_t *info, bool connected);

    void CAManagerAdapterStateChangedCB(CATransportAdapter_t adapter, bool enabled);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caManagerInitialize
     * Method:    caManagerInitialize
     * Signature: (Landroid/content/Context;)V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caManagerInitialize(JNIEnv *env, jclass clazz,
                                                         jobject context, jobject listener);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caManagerTerminate
     * Method:    caManagerTerminate
     * Signature: ()V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caManagerTerminate(JNIEnv *env, jclass clazz);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caManagerSetAutoConnectionDeviceInfo
     * Method:    caManagerSetAutoConnectionDeviceInfo
     * Signature: (Ljava/lang/String;)V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caManagerSetAutoConnectionDeviceInfo(JNIEnv *env,
                                                                          jclass clazz,
                                                                          jstring jaddress);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caManagerUnsetAutoConnectionDeviceInfo
     * Method:    caManagerUnsetAutoConnectionDeviceInfo
     * Signature: (Ljava/lang/String;)V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caManagerUnsetAutoConnectionDeviceInfo(JNIEnv *env,
                                                                            jclass clazz,
                                                                            jstring jaddress);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caBtPairingInitialize
     * Method:    caBtPairingInitialize
     * Signature: (Landroid/content/Context;)V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caBtPairingInitialize(JNIEnv *, jclass, jobject, jobject);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caBtPairingTerminate
     * Method:    caBtPairingTerminate
     * Signature: ()V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caBtPairingTerminate(JNIEnv *env, jclass clazz);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caBtPairingStartScan
     * Method:    caBtPairingStartScan
     * Signature: ()V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caBtPairingStartScan(JNIEnv *, jclass);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caBtPairingStopScan
     * Method:    caBtPairingStopScan
     * Signature: ()V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caBtPairingStopScan(JNIEnv *, jclass);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_caBtPairingCreateBond
     * Method:    caBtPairingCreateBond
     * Signature: (Landroid/bluetooth/BluetoothDevice;)V
     */
    JNIEXPORT void JNICALL
    Java_org_iotivity_ca_CaInterface_caBtPairingCreateBond(JNIEnv *, jclass, jobject);

    /*
     * Class:     org_iotivity_ca_CaInterface_Initialize
     * Method:    Initialize
     * Signature: (Landroid/content/Context;)V
     */
    JNIEXPORT void JNICALL Java_org_iotivity_ca_CaInterface_initialize
        (JNIEnv *, jclass, jobject, jobject);

    /*
     * Class:     Java_org_iotivity_ca_CaInterface_setLeScanIntervalTimeImpl
     * Method:    setLeScanIntervalTimeImpl
     * Signature: (II)V
     */
    JNIEXPORT void JNICALL Java_org_iotivity_ca_CaInterface_setLeScanIntervalTimeImpl
        (JNIEnv *, jclass, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
