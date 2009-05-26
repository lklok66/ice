// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice Touch is licensed to you under the terms described in the
// ICE_TOUCH_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Transceiver.h>
#include <EndpointI.h>

#include <Ice/Instance.h>
#include <Ice/Properties.h>
#include <Ice/TraceLevels.h>
#include <Ice/LoggerUtil.h>
#include <Ice/Stats.h>
#include <Ice/Buffer.h>
#include <Ice/Network.h>
#include <Ice/LocalException.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

void
Ice::CFNetworkException::ice_print(ostream& out) const
{
    Exception::ice_print(out);
    out << ":\nnetwork exception: domain: " << domain << " error: " << error;
}

static inline std::string
fromCFString(CFStringRef ref)
{
   const char* s = CFStringGetCStringPtr(ref, kCFStringEncodingUTF8);
   if(s)
   {
       return string(s);
   }

   // Not great, but is good enough for this purpose.
   char buf[1024];
   CFStringGetCString(ref, buf, sizeof(buf), kCFStringEncodingUTF8);
   return string(buf);
}

SOCKET
IceObjC::Transceiver::fd()
{
    assert(_fd != INVALID_SOCKET);
    return _fd;
}

void*
IceObjC::Transceiver::stream()
{
    assert(_readStream);
    return _readStream;
}

void
IceObjC::Transceiver::close()
{
    if(_traceLevels->network >= 1)
    {
        Trace out(_logger, _traceLevels->networkCat);
        out << "closing " << _instance->protocol() << " connection\n" << toString();
    }

    CFReadStreamClose(_readStream);
    CFWriteStreamClose(_writeStream);

    if(_fd != INVALID_SOCKET)
    {
        try
        {
            closeSocket(_fd);
            _fd = INVALID_SOCKET;
        }
        catch(const SocketException&)
        {
            _fd = INVALID_SOCKET;
            throw;
        }
    }
}

bool
IceObjC::Transceiver::write(Buffer& buf)
{
    // Its impossible for the packetSize to be more than an Int.
    int packetSize = static_cast<int>(buf.b.end() - buf.i);
    
    while(buf.i != buf.b.end())
    {
        if(!CFWriteStreamCanAcceptBytes(_writeStream) && CFWriteStreamGetStatus(_writeStream) != kCFStreamStatusError)
        {
            return false;
        }
        assert(CFWriteStreamGetStatus(_writeStream) >= kCFStreamStatusOpen);

        assert(_fd != INVALID_SOCKET);
        CFIndex ret = CFWriteStreamWrite(_writeStream, reinterpret_cast<const UInt8*>(&*buf.i), packetSize);

        if(ret == SOCKET_ERROR)
        {
            assert(CFWriteStreamGetStatus(_writeStream) == kCFStreamStatusError);
            CFErrorRef err = CFWriteStreamCopyError(_writeStream);
            CFStringRef domain = CFErrorGetDomain(err);
            if(CFStringCompare(domain, kCFErrorDomainPOSIX, 0) == kCFCompareEqualTo)
            {
                errno = CFErrorGetCode(err);
                CFRelease(err);
                
                if(interrupted())
                {
                    continue;
                }
                
                if(noBuffers() && packetSize > 1024)
                {
                    packetSize /= 2;
                    continue;
                }
                
                if(wouldBlock())
                {
                    return false;
                }
                
                if(connectionLost())
                {
                    ConnectionLostException ex(__FILE__, __LINE__);
                    ex.error = getSocketErrno();
                    throw ex;
                }
                else
                {
                    SocketException ex(__FILE__, __LINE__);
                    ex.error = getSocketErrno();
                    throw ex;
                }
            }
            else
            { 
                CFNetworkException ex(__FILE__, __LINE__);
                ex.domain = fromCFString(domain);                    
                ex.error = CFErrorGetCode(err);
                CFRelease(err);
                throw ex;                
            }
        }

        if(_traceLevels->network >= 3)
        {
            Trace out(_logger, _traceLevels->networkCat);
            out << "sent " << ret << " of " << packetSize << " bytes via " << _instance->protocol() << "\n" << toString();
        }

        if(_stats)
        {
            _stats->bytesSent(type(), static_cast<Int>(ret));
        }

        buf.i += ret;

        if(packetSize > buf.b.end() - buf.i)
        {
            packetSize = static_cast<int>(buf.b.end() - buf.i);
        }
    }

    return true;
}

bool
IceObjC::Transceiver::read(Buffer& buf)
{
    // Its impossible for the packetSize to be more than an Int.
    int packetSize = static_cast<int>(buf.b.end() - buf.i);
    
    while(buf.i != buf.b.end())
    {
        if(!CFReadStreamHasBytesAvailable(_readStream) && CFReadStreamGetStatus(_readStream) != kCFStreamStatusError)
        {
            return false;
        }
        assert(CFReadStreamGetStatus(_readStream) >= kCFStreamStatusOpen);

        assert(_fd != INVALID_SOCKET);
        CFIndex ret = CFReadStreamRead(_readStream, reinterpret_cast<UInt8*>(&*buf.i), packetSize);

        if(ret == 0)
        {
            //
            // If the connection is lost when reading data, we shut
            // down the write end of the socket. This helps to unblock
            // threads that are stuck in send() or select() while
            // sending data. Note: I don't really understand why
            // send() or select() sometimes don't detect a connection
            // loss. Therefore this helper to make them detect it.
            //
            //assert(_fd != INVALID_SOCKET);
            //shutdownSocketReadWrite(_fd);
            
            ConnectionLostException ex(__FILE__, __LINE__);
            ex.error = 0;
            throw ex;
        }

        if(ret == SOCKET_ERROR)
        {
            assert(CFReadStreamGetStatus(_readStream) == kCFStreamStatusError);
            CFErrorRef err = CFReadStreamCopyError(_readStream);
            CFStringRef domain = CFErrorGetDomain(err);
            if(CFStringCompare(domain, kCFErrorDomainPOSIX, 0) == kCFCompareEqualTo)
            {
                errno = CFErrorGetCode(err);
                CFRelease(err);
                
                if(interrupted())
                {
                    continue;
                }
                
                if(noBuffers() && packetSize > 1024)
                {
                    packetSize /= 2;
                    continue;
                }
                
                if(wouldBlock())
                {
                    return false;
                }
                
                if(connectionLost())
                {
                    //
                    // See the commment above about shutting down the
                    // socket if the connection is lost while reading
                    // data.
                    //
                    //assert(_fd != INVALID_SOCKET);
                    //shutdownSocketReadWrite(_fd);
                    
                    ConnectionLostException ex(__FILE__, __LINE__);
                    ex.error = getSocketErrno();
                    throw ex;
                }
                else
                {
                    SocketException ex(__FILE__, __LINE__);
                    ex.error = getSocketErrno();
                    throw ex;
                }
            }
            else
            {
                CFNetworkException ex(__FILE__, __LINE__);
                ex.domain = fromCFString(domain);                    
                ex.error = CFErrorGetCode(err);
                CFRelease(err);
                throw ex;                
            }
        }

        if(_traceLevels->network >= 3)
        {
            Trace out(_logger, _traceLevels->networkCat);
            out << "received " << ret << " of " << packetSize << " bytes via " << _instance->protocol() << "\n" << toString();
        }

        if(_stats)
        {
            _stats->bytesReceived(type(), static_cast<Int>(ret));
        }

        buf.i += ret;

        if(packetSize > buf.b.end() - buf.i)
        {
            packetSize = static_cast<int>(buf.b.end() - buf.i);
        }
    }

    return true;
}

bool
IceObjC::Transceiver::hasMoreData()
{
    return CFReadStreamHasBytesAvailable(_readStream) || CFReadStreamGetStatus(_readStream) == kCFStreamStatusError;
}

string
IceObjC::Transceiver::type() const
{
    return _instance->protocol();
}

string
IceObjC::Transceiver::toString() const
{
    return _desc;
}

SocketStatus
IceObjC::Transceiver::initialize()
{
    if(_state == StateNeedConnect)
    {
        _state = StateConnectPending;
        return NeedConnect;
    }
    else if(_state <= StateConnectPending)
    {
        try
        {
            CFStreamStatus status = CFReadStreamGetStatus(_readStream);
            assert(status > kCFStreamStatusOpening);

            if(status == kCFStreamStatusError)
            {
                CFErrorRef err = CFReadStreamCopyError(_readStream);
                CFStringRef domain = CFErrorGetDomain(err);
                if(CFStringCompare(domain, kCFErrorDomainPOSIX, 0) == kCFCompareEqualTo)
                {
                    errno = CFErrorGetCode(err);
                    CFRelease(err);
                    if(connectionRefused())
                    {
                        ConnectionRefusedException ex(__FILE__, __LINE__);
                        ex.error = getSocketErrno();
                        throw ex;
                    }
                    else if(connectFailed())
                    {
                        ConnectFailedException ex(__FILE__, __LINE__);
                        ex.error = getSocketErrno();
                        throw ex;
                    }
                    else
                    {
                        SocketException ex(__FILE__, __LINE__);
                        ex.error = getSocketErrno();
                        throw ex;
                    }
                }
                else if(CFStringCompare(domain, kCFErrorDomainCFNetwork, 0) == kCFCompareEqualTo)
                {
                    int error = CFErrorGetCode(err);
                    if(error == kCFHostErrorHostNotFound || error == kCFHostErrorUnknown)
                    {
                        int rs = 0;
                        if(error == kCFHostErrorUnknown)
                        {
                            CFDictionaryRef dict = CFErrorCopyUserInfo(err);
                            CFNumberRef d = (CFNumberRef)CFDictionaryGetValue(dict, kCFGetAddrInfoFailureKey);
                            if(d != 0)
                            {
                                CFNumberGetValue(d, kCFNumberSInt32Type, &rs);
                            }
                            CFRelease(dict);
                        }
                        
                        CFRelease(err);
                        
                        DNSException ex(__FILE__, __LINE__);
                        ex.error = rs;
                        ex.host = _host;
                        
                        throw ex;
                    }
                }
                
                // Otherwise throw a generic exception.    
                CFNetworkException ex(__FILE__, __LINE__);
                ex.domain = fromCFString(domain);                    
                ex.error = CFErrorGetCode(err);
                CFRelease(err);
                throw ex;
            }

            assert(status == kCFStreamStatusOpen);
            _state = StateConnected;

            if(_fd == INVALID_SOCKET)
            {
                if(!CFReadStreamSetProperty(_readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanFalse) || 
                   !CFWriteStreamSetProperty(_writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanFalse))
                {
                    throw Ice::SocketException(__FILE__, __LINE__, 0);
                }
            
                CFDataRef d = (CFDataRef)CFReadStreamCopyProperty(_readStream, kCFStreamPropertySocketNativeHandle);
                CFDataGetBytes(d, CFRangeMake(0, sizeof(SOCKET)), reinterpret_cast<UInt8*>(&_fd));
                CFRelease(d);
            }

            _desc = fdToString(_fd);

            setBlock(_fd, false);
            setTcpBufSize(_fd, _instance->initializationData().properties, _logger);

            checkCertificates();

            CFWriteStreamOpen(_writeStream);
            assert(CFWriteStreamGetStatus(_writeStream) > kCFStreamStatusOpening);
        }
        catch(const Ice::LocalException& ex)
        {
            if(_traceLevels->network >= 2)
            {
                Trace out(_logger, _traceLevels->networkCat);
                out << "failed to establish " << _instance->protocol() << " connection\n" << _desc << "\n" << ex;
            }
            throw;
        }

        if(_traceLevels->network >= 1)
        {
            Trace out(_logger, _traceLevels->networkCat);
            if(_host.empty())
            {
                out << _instance->protocol() << " connection accepted\n" << _desc;
            }
            else
            {
                out << _instance->protocol() << " connection established\n" << _desc;
            }
        }
    }
    assert(_state == StateConnected);
    return Finished;
}

void
IceObjC::Transceiver::checkSendSize(const Buffer& buf, size_t messageSizeMax)
{
    if(buf.b.size() > messageSizeMax)
    {
        throw MemoryLimitException(__FILE__, __LINE__);
    }
}

IceObjC::Transceiver::Transceiver(const InstancePtr& instance, 
                                  CFReadStreamRef readStream,
                                  CFWriteStreamRef writeStream,
                                  const string& host,
                                  Ice::Int port) :
    _instance(instance),
    _traceLevels(instance->traceLevels()),
    _logger(instance->initializationData().logger),
    _stats(instance->initializationData().stats),
    _host(host),
    _fd(INVALID_SOCKET),
    _readStream(readStream),
    _writeStream(writeStream),
    _state(StateNeedConnect)
{
    ostringstream s;
    s << "local address = <not available>";
    s << "\nremote address = " << host << ":" << port;
    _desc = s.str();
}

IceObjC::Transceiver::Transceiver(const InstancePtr& instance, 
                                  CFReadStreamRef readStream,
                                  CFWriteStreamRef writeStream,
                                  SOCKET fd) :
    _instance(instance),
    _traceLevels(instance->traceLevels()),
    _logger(instance->initializationData().logger),
    _stats(instance->initializationData().stats),
    _fd(fd),
    _readStream(readStream),
    _writeStream(writeStream),
    _state(StateNeedConnect),
    _desc(fdToString(fd))
{
}

IceObjC::Transceiver::~Transceiver()
{
    assert(_fd == INVALID_SOCKET);
    CFRelease(_readStream);
    CFRelease(_writeStream);
}

void
IceObjC::Transceiver::checkCertificates()
{
    CFArrayRef certificates = (CFArrayRef)CFWriteStreamCopyProperty(_writeStream, kCFStreamPropertySSLPeerCertificates);
    if(certificates)
    {
        OSStatus err;
        bool checkCertName = !_host.empty() &&
            _instance->initializationData().properties->getPropertyAsIntWithDefault("IceSSL.CheckCertName", 1);
#if !TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR && !__IPHONE_3_0
        SecPolicySearchRef policySearch = 0;
        SecPolicyRef policy = 0;
        const CSSM_OID* oid = checkCertName ? &CSSMOID_APPLE_TP_SSL : &CSSMOID_APPLE_X509_BASIC;
        if((err = SecPolicySearchCreate(CSSM_CERT_X_509v3, oid, NULL, &policySearch)) != noErr ||
           (err = SecPolicySearchCopyNext(policySearch, &policy)) != noErr)
        {
            if(policySearch)
            {
                CFRelease(policySearch);
            }
            CFRelease(certificates);
            std::ostringstream os;
            os << "unable to create policy object (error = " << err << ")";
            throw Ice::SecurityException(__FILE__, __LINE__, os.str());
        }
        CFRelease(policySearch);

        if(checkCertName)
        {
            CSSM_APPLE_TP_SSL_OPTIONS opts;
            memset(&opts, 0, sizeof(opts));
            opts.Version = CSSM_APPLE_TP_SSL_OPTS_VERSION;
            opts.ServerNameLen = _host.size();
            opts.ServerName = _host.c_str();
            opts.Flags = CSSM_APPLE_TP_SSL_CLIENT;
            CSSM_DATA optsData = { sizeof(opts), (uint8 *)&opts };
            SecPolicySetValue(policy, &optsData);
        }
#else
        SecPolicyRef policy;
        if(!checkCertName)
        {
            policy = SecPolicyCreateBasicX509();
        }
        else
        {
            CFStringRef h = CFStringCreateWithCString(NULL, _host.c_str(), kCFStringEncodingUTF8);
            policy = SecPolicyCreateSSL(false, h);
            CFRelease(h);
        }
#endif
        SecTrustRef trust;
        SecTrustResultType result;
        if((err = SecTrustCreateWithCertificates(certificates, policy, &trust)) != noErr)
        {
            CFRelease(policy);
            CFRelease(certificates);
            std::ostringstream os;
            os << "unable to ceate trust object with peer certificates (error = " << err << ")";
            throw Ice::SecurityException(__FILE__, __LINE__, os.str());
        }
        CFRelease(policy);

        //
        // If IceSSL.CertAuthFile is set, we use the certificate authorities from this file
        // instead of the ones from the keychain.
        //
        if(_instance->certificateAuthorities())
        {
            if((err = SecTrustSetAnchorCertificates(trust, _instance->certificateAuthorities())) != noErr)
            {
                std::ostringstream os;
                os << "couldn't set root CA certificates with trust object (error = " << err << ")";
                throw Ice::SecurityException(__FILE__, __LINE__, os.str());
            }
        }

        if((err = SecTrustEvaluate(trust, &result)) != noErr)
        {
            CFRelease(trust);
            CFRelease(certificates);
            std::ostringstream os;
            os << "unable to evaluate the peer certificate trust (error = " << err << ")";
            throw Ice::SecurityException(__FILE__, __LINE__, os.str());
        }

        CFRelease(trust);

        //
        // The kSecTrustResultUnspecified result indicates that the user didn't set any trust
        // settings for the root CA. This is expected if the root CA is provided by the user
        // with IceSSL.CertAuthFile or if the user didn't explicitly set any trust settings
        // for the certificate.
        //
        if(result != kSecTrustResultProceed && result != kSecTrustResultUnspecified)
        {
            CFRelease(certificates);
            std::ostringstream os;
            os << "certificate validation failed (result = " << result << ")";
            throw Ice::SecurityException(__FILE__, __LINE__, os.str());
        }

        if(_instance->trustOnlyKeyID())
        {
#if !TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR && !__IPHONE_3_0
            SecCertificateRef cert = (SecCertificateRef)CFArrayGetValueAtIndex(certificates, 0);

            CSSM_CL_HANDLE handle;
            CSSM_DATA data;
            if((err = SecCertificateGetCLHandle(cert, &handle)) != noErr ||
               (err = SecCertificateGetData(cert, &data)) != noErr)
            {
                CFRelease(certificates);
                std::ostringstream os;
                os << "couldn't obtain certificate information to check the subject key ID (error = " << err << ")";
                throw Ice::SecurityException(__FILE__, __LINE__, os.str());
            }
       
            const CSSM_OID* tag = &CSSMOID_SubjectKeyIdentifier;
            CSSM_DATA *result;
            uint32 count;
            CSSM_HANDLE moreResults;
            CSSM_RETURN error = CSSM_CL_CertGetFirstFieldValue(handle, &data, tag, &moreResults, &count, &result);
            if(error != CSSM_OK)
            {
                CFRelease(certificates);
                std::ostringstream os;
                os << "couldn't obtain certificate information to check the subject key ID (error = " << error << ")";
                throw Ice::SecurityException(__FILE__, __LINE__, os.str());
            }

            CSSM_X509_EXTENSION_PTR ext = (CSSM_X509_EXTENSION_PTR)result->Data;
            if(ext->format != CSSM_X509_DATAFORMAT_PARSED)
            {
                CSSM_CL_CertAbortQuery(handle, moreResults);
                CSSM_CL_FreeFieldValue(handle, tag, result);
                CFRelease(certificates);
                std::ostringstream os;
                os << "unexpected format for subject key ID (format = " << ext->format << ")";
                throw Ice::SecurityException(__FILE__, __LINE__, os.str());
            }

            CE_SubjectKeyID* peerKey = (CE_SubjectKeyID*)ext->value.parsedValue;
            CFDataRef key = _instance->trustOnlyKeyID();
            if(peerKey->Length != (uint32)CFDataGetLength(key) ||
               memcmp(peerKey->Data, CFDataGetBytePtr(key), peerKey->Length) != 0)
            {
                CSSM_CL_CertAbortQuery(handle, moreResults);
                CSSM_CL_FreeFieldValue(handle, tag, result);
                CFRelease(certificates);
                std::ostringstream os;
                os << "the certificate subject key ID doesn't match the `IceSSL.TrustOnly.Client' property";
                throw Ice::SecurityException(__FILE__, __LINE__, os.str());
            }

            CSSM_CL_CertAbortQuery(handle, moreResults);
            CSSM_CL_FreeFieldValue(handle, tag, result);
#else
            //
            // To check the subject key ID, we add the peer certificate to the keychain with SetItemAdd,
            // then we lookup for the cert using the kSecAttrSubjectKeyID. Then we remove the cert from
            // the keychain. NOTE: according to the Apple documentation, it should in theory be possible
            // to not add/remove the item to the keychain by specifying the kSecMatchItemList key (or 
            // kSecUseItemList?) when calling SecItemCopyMatching. Unfortunately this doesn't appear to
            // work. Similarly, it should be possible to get back the attributes of the certificate 
            // once it added by setting kSecReturnAttributes in the add query, again this doesn't seem
            // to work.
            //

            CFMutableDictionaryRef query;
            query = CFDictionaryCreateMutable(0, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
            CFDictionarySetValue(query, kSecClass, kSecClassCertificate);
            CFDictionarySetValue(query, kSecValueRef, (SecCertificateRef)CFArrayGetValueAtIndex(certificates, 0));
            err = SecItemAdd(query, 0);
            if(err != noErr && err != errSecDuplicateItem)
            {
                CFRelease(query);
                CFRelease(certificates);
                std::ostringstream os;
                os << "unable to add peer certificate to keychain (error = " << err << ")";
                throw Ice::SecurityException(__FILE__, __LINE__, os.str());
            }
            CFRelease(query);

            query = CFDictionaryCreateMutable(0, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
            CFDictionarySetValue(query, kSecClass, kSecClassCertificate);
            CFDictionarySetValue(query, kSecValueRef, (SecCertificateRef)CFArrayGetValueAtIndex(certificates, 0));
            CFDictionarySetValue(query, kSecAttrSubjectKeyID, _instance->trustOnlyKeyID());
            err = SecItemCopyMatching(query, 0);
            OSStatus foundErr = err;
            CFRelease(query);

            query = CFDictionaryCreateMutable(0, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
            CFDictionarySetValue(query, kSecClass, kSecClassCertificate);
            CFDictionarySetValue(query, kSecValueRef, CFArrayGetValueAtIndex(certificates, 0));
            err = SecItemDelete(query);
            if(err != noErr)
            {
                CFRelease(certificates);
                CFRelease(query);
                std::ostringstream os;
                os << "unable to remove peer certificate from keychain (error = " << err << ")";
                throw Ice::SecurityException(__FILE__, __LINE__, os.str());
            }
            CFRelease(query);

            if(foundErr != noErr)
            {
                CFRelease(certificates);
                std::ostringstream os;
                os << "the certificate subject key ID doesn't match the `IceSSL.TrustOnly.Client' property (error = ";
                os << foundErr << ")";
                throw Ice::SecurityException(__FILE__, __LINE__, os.str());
            }
#endif
        }
        CFRelease(certificates);
    }
}
