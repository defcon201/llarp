#ifndef LLARP_SERVICE_PROTOCOL_HPP
#define LLARP_SERVICE_PROTOCOL_HPP
#include <llarp/time.h>
#include <llarp/bencode.hpp>
#include <llarp/crypto.hpp>
#include <llarp/encrypted.hpp>
#include <llarp/routing/message.hpp>
#include <llarp/service/Info.hpp>
#include <llarp/service/Intro.hpp>
#include <vector>

namespace llarp
{
  namespace service
  {
    constexpr std::size_t MAX_PROTOCOL_MESSAGE_SIZE = 2048;

    enum ProtocolType
    {
      eProtocolText    = 0,
      eProtocolTraffic = 1
    };

    /// inner message
    struct ProtocolMessage : public llarp::IBEncodeMessage
    {
      ProtocolMessage();
      ~ProtocolMessage();
      ProtocolType proto;
      llarp_time_t queued = 0;
      std::vector< byte_t > payload;
      Introduction introReply;
      ServiceInfo sender;

      bool
      DecodeKey(llarp_buffer_t key, llarp_buffer_t* val);
      bool
      BEncode(llarp_buffer_t* buf) const;

      void
      PutBuffer(llarp_buffer_t payload);
    };

    /// outer message
    struct ProtocolFrame : public llarp::routing::IMessage
    {
      llarp::Encrypted D;
      llarp::PubKey H;
      llarp::KeyExchangeNonce N;
      llarp::Signature Z;

      ~ProtocolFrame();

      bool
      EncryptAndSign(llarp_crypto* c, const ProtocolMessage* msg,
                     byte_t* sharedkey, byte_t* signingkey);

      bool
      DecryptPayloadInto(llarp_crypto* c, byte_t* sharedkey,
                         ProtocolMessage* into) const;

      bool
      DecodeKey(llarp_buffer_t key, llarp_buffer_t* val);

      bool
      BEncode(llarp_buffer_t* buf) const;

      bool
      Verify(llarp_crypto* c, byte_t* signingkey);

      bool
      HandleMessage(llarp::routing::IMessageHandler* h, llarp_router* r) const;
    };
  }  // namespace service
}  // namespace llarp

#endif