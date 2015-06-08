#pragma once

#include "zyre.h"

#include <string>
#include <exception>
#include <vector>

namespace zyre
{
    class error_t : public std::runtime_error
    {
    public:
        error_t(const std::string& what) : std::runtime_error(what) {}
    };

    class event_t
    {
    public:
        event_t(zyre_event_t* self) : m_self(self) {};

        ~event_t()
        {
            if (m_self)
                zyre_event_destroy(&m_self);
        }

        event_t(const event_t& other) = delete;
        event_t operator=(const event_t& other) = delete;

        event_t(event_t&& other)
        {
            m_self = other.m_self;
            other.m_self = nullptr;
        }

        event_t& operator=(event_t&& other)
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

        std::string header_value(const std::string& key) const
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

    class node_t
    {
    public:
        node_t(const std::string& name = "")
        {
            if (name != "")
                m_self = zyre_new(NULL);
            else
                m_self = zyre_new(name.c_str());
        }

        ~node_t()
        {
            if (m_self)
                zyre_destroy(&m_self);
        }

        node_t(const node_t& other) = delete;
        node_t operator=(const node_t& other) = delete;

        node_t(node_t&& other)
        {
            m_self = other.m_self;
            other.m_self = nullptr;
        }

        node_t& operator=(node_t&& other)
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

        void set_header(const std::string key, const std::string& value) const
        {
            zyre_set_header(m_self, key.c_str(), value.c_str());
        }

        void set_verbose() const
        {
            zyre_set_verbose(m_self);
        }

        void set_port(int value) const
        {
            zyre_set_port(m_self, value);
        }

        void set_interval(size_t value) const
        {
            zyre_set_interval(m_self, value);
        }

        void set_interface(const std::string& value) const
        {
            zyre_set_interface(m_self, value.c_str());
        }

        void start() const
        {
            int rc = zyre_start(m_self);
            if (rc == -1)
                throw error_t("Failed to start Zyre node");
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

        zmsg_t* recv() const
        {
            return zyre_recv(m_self);
        }

        event_t event() const
        {
            return event_t(zyre_event_new(m_self));
        }

        std::vector<std::string> peers() const
        {
            zlist_t* peers = zyre_peers(m_self);
            std::vector<std::string> ret = to_vector(peers);
            zlist_destroy(&peers);
            return ret;
        }        

        std::vector<std::string> own_groups() const
        {
            zlist_t* ownGroups = zyre_own_groups(m_self);
            std::vector<std::string> ret = to_vector(ownGroups);
            zlist_destroy(&ownGroups);
            return ret;
        }

        std::vector<std::string> peer_groups() const
        {
            zlist_t* peerGroups = zyre_peer_groups(m_self);
            std::vector<std::string> ret = to_vector(peerGroups);
            zlist_destroy(&peerGroups);
            return ret;
        }

        std::string peer_address(const std::string& peer) const
        {
            char* val = zyre_peer_address(m_self, peer.c_str());
            std::string ret(val);
            if (val != NULL)
                delete val;
            return ret;
        }

        std::string peer_header_value(const std::string& peer, const std::string& name)
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
        std::vector<std::string> to_vector(zlist_t* list) const
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