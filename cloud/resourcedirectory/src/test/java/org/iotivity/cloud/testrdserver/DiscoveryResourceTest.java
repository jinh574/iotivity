/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.testrdserver;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.rdserver.resources.directory.rd.ResourceDirectoryResource;
import org.iotivity.cloud.rdserver.resources.directory.res.DiscoveryResource;
import org.iotivity.cloud.util.Cbor;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mockito;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

public class DiscoveryResourceTest {
    private Cbor<ArrayList<Object>>   mCbor              = new Cbor<>();
    private ResourceDirectoryResource mRDResource        = null;
    private DiscoveryResource         mDiscoveryResource = null;
    private CoapDevice                mockDevice         = null;
    CountDownLatch                    latch              = null;
    IResponse                         res;

    @Before
    public void setUp() throws Exception {
        res = null;
        mockDevice = mock(CoapDevice.class);
        latch = new CountDownLatch(1);
        mRDResource = new ResourceDirectoryResource();
        mDiscoveryResource = new DiscoveryResource();
        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                CoapResponse resp = (CoapResponse) invocation.getArguments()[0];
                latch.countDown();
                res = resp;
                return resp;
            }
        }).when(mockDevice).sendResponse(Mockito.anyObject());
    }

    @After
    public void tearDown() throws Exception {
        RDServerTestUtils.resetRDDatabase();
    }

    @Test
    public void testHandleGetRequest_notExistVaule() throws Exception {
        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                RDServerTestUtils.DISCOVERY_REQ_URI,
                "rt=core.light;di=" + RDServerTestUtils.DI);
        mDiscoveryResource.onDefaultRequestReceived(mockDevice, request);
        // assertion: if the response status is "CONTENT"
        // assertion : if the payload is null
        assertTrue(latch.await(2L, SECONDS));
        assertTrue(methodCheck(res, ResponseStatus.CONTENT));
        assertTrue(nullPayloadCheck(res));
    }

    @Test
    public void testHandleGetRequest_existValue() throws Exception {
        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                RDServerTestUtils.DISCOVERY_REQ_URI,
                "rt=core.light;di=" + RDServerTestUtils.DI);
        mRDResource.onDefaultRequestReceived(mockDevice,
                RDServerTestUtils.makePublishRequest());
        mDiscoveryResource.onDefaultRequestReceived(mockDevice, request);
        // assertion: if the response status is "CONTENT"
        // assertion : if the payload contains resource info
        assertTrue(latch.await(2L, SECONDS));
        assertTrue(methodCheck(res, ResponseStatus.CONTENT));
        assertTrue(discoverHashmapCheck(res, "di"));
        assertTrue(discoveredResourceCheck(res, "href"));
        assertTrue(discoveredResourceCheck(res, "rt"));
        assertTrue(discoveredResourceCheck(res, "if"));
    }

    private boolean discoverHashmapCheck(IResponse response,
            String propertyName) {
        ArrayList<Object> resourceList = mCbor
                .parsePayloadFromCbor(response.getPayload(), ArrayList.class);
        HashMap<Object, Object> firstArray = (HashMap<Object, Object>) resourceList
                .get(0);
        if (firstArray.get(propertyName) != null)
            return true;
        else
            return false;
    }

    private boolean discoveredResourceCheck(IResponse response,
            String propertyName) {
        ArrayList<Object> resourceList = mCbor
                .parsePayloadFromCbor(response.getPayload(), ArrayList.class);
        HashMap<Object, Object> firstArray = (HashMap<Object, Object>) resourceList
                .get(0);
        ArrayList<HashMap<Object, Object>> linkData = (ArrayList<HashMap<Object, Object>>) firstArray
                .get("links");
        HashMap<Object, Object> linkMap = linkData.get(0);
        if (linkMap.get(propertyName) != null)
            return true;
        else
            return false;
    }

    private boolean methodCheck(IResponse response,
            ResponseStatus responseStatus) {
        if (responseStatus == response.getStatus())
            return true;
        else
            return false;
    }

    private boolean nullPayloadCheck(IResponse response) {
        ArrayList<Object> payloadData = mCbor
                .parsePayloadFromCbor(res.getPayload(), ArrayList.class);
        return (payloadData.isEmpty());
    }
}