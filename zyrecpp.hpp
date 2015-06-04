#pragma once

#include "zyre.h"

#include <string>
#include <exception>
#include <vector>

namespace Zyre
{
    class ZyreError : public std::runtime_error
    {
    public:
        ZyreError(const std::string& what) : std::runtime_error(what) {}
    };

    class ZyreEvent
    {
    public:
        ZyreEvent(zyre_event_t* self) : m_self(self) {};

        ~ZyreEvent()
        {
            if (m_self)
                zyre_event_destroy(&m_self);
        }

        ZyreEvent(const ZyreEvent& other) = delete;
        ZyreEvent operator=(const ZyreEvent& other) = delete;

        ZyreEvent(ZyreEvent&& other)
        {
            m_self = other.m_self;
            other.m_self = nullptr;
        }

        ZyreEvent& operator=(ZyreEvent&& other)
        {
            if (&other != this)
            {
                m_self = other.m_self;
                other.m_self = nullptr;
            }
            return *this;
        }

        void print() const
        {
            zyre_event_print(m_self);
        }

        zyre_event_type_t type() const
        {
            return zyre_event_type(m_self);
        }

        std::string sender() const
        {
            return zyre_event_sender(m_self);
        }

        std::string name() const
        {
            return zyre_event_name(m_self);
        }

        std::string address() const
        {
            return zyre_event_address(m_self);
        }

        std::string headerValue(const std::string& key) const
        {
            return zyre_event_header(m_self, key.c_str());
        }

        std::string group() const
        {
            return zyre_event_group(m_self);
        }

        zmsg_t* message() const
        {
            return zyre_event_msg(m_self);
        }

    private:
        zyre_event_t* m_self;
    };

    class Zyre
    {
    public:
        Zyre(const std::string& name = "")
        {
            if (name != "")
                m_self = zyre_new(NULL);
            else
                m_self = zyre_new(name.c_str());
        }

        ~Zyre()
        {
            if (m_self)
                zyre_destroy(&m_self);
        }

        Zyre(const Zyre& other) = delete;
        Zyre operator=(const Zyre& other) = delete;

        Zyre(Zyre&& other)
        {
            m_self = other.m_self;
            other.m_self = nullptr;
        }

        Zyre& operator=(Zyre&& other)
        {
            if (&other != this)
            {
                m_self = other.m_self;
                other.m_self = nullptr;
            }
            return *this;
        }

        void print() const
        {
            zyre_print(m_self);
        }

        std::string uuid() const
        {
            return zyre_uuid(m_self);
        }

        std::string name() const
        {
            return zyre_name(m_self);
        }

        void setHeader(const std::string key, const std::string& value) const
        {
            zyre_set_header(m_self, key.c_str(), value.c_str());
        }

        void setVerbose() const
        {
            zyre_set_verbose(m_self);
        }

        void setPort(int value) const
        {
            zyre_set_port(m_self, value);
        }

        void setInterval(size_t value) const
        {
            zyre_set_interval(m_self, value);
        }

        void setInterface(const std::string& value) const
        {
            zyre_set_interface(m_self, value.c_str());
        }

        void start() const
        {
            int rc = zyre_start(m_self);
            if (rc == -1)
                throw ZyreError("Failed to start Zyre node");
        }

        void stop() const
        {
            zyre_stop(m_self);
        }

        void join(const std::string& group) const
        {
            zyre_join(m_self, group.c_str());
        }

        void leave(const std::string& group) const
        {
            zyre_leave(m_self, group.c_str());
        }

        void whisper(const std::string& peer, zmsg_t* msg) const
        {
            zyre_whisper(m_self, peer.c_str(), &msg);
        }

        void shout(const std::string& group, zmsg_t* msg) const
        {
            zyre_shout(m_self, group.c_str(), &msg);
        }

        zmsg_t* receive() const
        {
            return zyre_recv(m_self);
        }

        ZyreEvent event() const
        {
            return ZyreEvent(zyre_event_new(m_self));
        }

        std::vector<std::string> peers() const
        {
            zlist_t* peers = zyre_peers(m_self);
            std::vector<std::string> ret = toVector(peers);
            zlist_destroy(&peers);
            return ret;
        }        

        std::vector<std::string> ownGroups() const
        {
            zlist_t* ownGroups = zyre_own_groups(m_self);
            std::vector<std::string> ret = toVector(ownGroups);
            zlist_destroy(&ownGroups);
            return ret;
        }

        std::vector<std::string> peerGroups() const
        {
            zlist_t* peerGroups = zyre_peer_groups(m_self);
            std::vector<std::string> ret = toVector(peerGroups);
            zlist_destroy(&peerGroups);
            return ret;
        }

        std::string peerAddress(const std::string& peer) const
        {
            char* val = zyre_peer_address(m_self, peer.c_str());
            std::string ret(val);
            if (val != NULL)
                delete val;
            return ret;
        }

        std::string peerHeaderValue(const std::string& peer, const std::string& name)
        {
            char* val = zyre_peer_header_value(m_self, peer.c_str(), peer.c_str());
            std::string ret(val);
            if (val != NULL)
                delete val;
            return ret;
        }

        zsock_t* socket() const
        {
            return zyre_socket(m_self);
        }

        static void version(int& major, int& minor, int& patch)
        {
            zyre_version(&major, &minor, &patch);
        }

    private:
        std::vector<std::string> toVector(zlist_t* list) const
        {
            std::vector<std::string> ret;
            void* cursor = zlist_first(list);
            while (cursor != NULL)
            {
                ret.emplace_back(static_cast<char*>(cursor));
                cursor = zlist_next(list);
            }
            return ret;
        }

        zyre_t* m_self;
    };
}