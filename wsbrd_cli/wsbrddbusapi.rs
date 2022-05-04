// This code was autogenerated with `dbus-codegen-rust -g -m None -d com.silabs.Wisun.BorderRouter -p /com/silabs/Wisun/BorderRouter`, see https://github.com/diwic/dbus-rs
use dbus as dbus;
#[allow(unused_imports)]
use dbus::arg;
use dbus::blocking;

pub trait OrgFreedesktopDBusPeer {
    fn ping(&self) -> Result<(), dbus::Error>;
    fn get_machine_id(&self) -> Result<String, dbus::Error>;
}

impl<'a, T: blocking::BlockingSender, C: ::std::ops::Deref<Target=T>> OrgFreedesktopDBusPeer for blocking::Proxy<'a, C> {

    fn ping(&self) -> Result<(), dbus::Error> {
        self.method_call("org.freedesktop.DBus.Peer", "Ping", ())
    }

    fn get_machine_id(&self) -> Result<String, dbus::Error> {
        self.method_call("org.freedesktop.DBus.Peer", "GetMachineId", ())
            .and_then(|r: (String, )| Ok(r.0, ))
    }
}

pub trait OrgFreedesktopDBusIntrospectable {
    fn introspect(&self) -> Result<String, dbus::Error>;
}

impl<'a, T: blocking::BlockingSender, C: ::std::ops::Deref<Target=T>> OrgFreedesktopDBusIntrospectable for blocking::Proxy<'a, C> {

    fn introspect(&self) -> Result<String, dbus::Error> {
        self.method_call("org.freedesktop.DBus.Introspectable", "Introspect", ())
            .and_then(|r: (String, )| Ok(r.0, ))
    }
}

pub trait OrgFreedesktopDBusProperties {
    fn get<R0: for<'b> arg::Get<'b> + 'static>(&self, interface_name: &str, property_name: &str) -> Result<R0, dbus::Error>;
    fn get_all(&self, interface_name: &str) -> Result<arg::PropMap, dbus::Error>;
    fn set<I2: arg::Arg + arg::Append>(&self, interface_name: &str, property_name: &str, value: I2) -> Result<(), dbus::Error>;
}

#[derive(Debug)]
pub struct OrgFreedesktopDBusPropertiesPropertiesChanged {
    pub interface_name: String,
    pub changed_properties: arg::PropMap,
    pub invalidated_properties: Vec<String>,
}

impl arg::AppendAll for OrgFreedesktopDBusPropertiesPropertiesChanged {
    fn append(&self, i: &mut arg::IterAppend) {
        arg::RefArg::append(&self.interface_name, i);
        arg::RefArg::append(&self.changed_properties, i);
        arg::RefArg::append(&self.invalidated_properties, i);
    }
}

impl arg::ReadAll for OrgFreedesktopDBusPropertiesPropertiesChanged {
    fn read(i: &mut arg::Iter) -> Result<Self, arg::TypeMismatchError> {
        Ok(OrgFreedesktopDBusPropertiesPropertiesChanged {
            interface_name: i.read()?,
            changed_properties: i.read()?,
            invalidated_properties: i.read()?,
        })
    }
}

impl dbus::message::SignalArgs for OrgFreedesktopDBusPropertiesPropertiesChanged {
    const NAME: &'static str = "PropertiesChanged";
    const INTERFACE: &'static str = "org.freedesktop.DBus.Properties";
}

impl<'a, T: blocking::BlockingSender, C: ::std::ops::Deref<Target=T>> OrgFreedesktopDBusProperties for blocking::Proxy<'a, C> {

    fn get<R0: for<'b> arg::Get<'b> + 'static>(&self, interface_name: &str, property_name: &str) -> Result<R0, dbus::Error> {
        self.method_call("org.freedesktop.DBus.Properties", "Get", (interface_name, property_name, ))
            .and_then(|r: (arg::Variant<R0>, )| Ok((r.0).0, ))
    }

    fn get_all(&self, interface_name: &str) -> Result<arg::PropMap, dbus::Error> {
        self.method_call("org.freedesktop.DBus.Properties", "GetAll", (interface_name, ))
            .and_then(|r: (arg::PropMap, )| Ok(r.0, ))
    }

    fn set<I2: arg::Arg + arg::Append>(&self, interface_name: &str, property_name: &str, value: I2) -> Result<(), dbus::Error> {
        self.method_call("org.freedesktop.DBus.Properties", "Set", (interface_name, property_name, arg::Variant(value), ))
    }
}

pub trait ComSilabsWisunBorderRouter {
    fn debug_ping(&self, arg0: &str) -> Result<(), dbus::Error>;
    fn add_root_certificate(&self, arg0: &str) -> Result<(), dbus::Error>;
    fn remove_root_certificate(&self, arg0: &str) -> Result<(), dbus::Error>;
    fn revoke_node(&self, arg0: Vec<u8>) -> Result<(), dbus::Error>;
    fn revoke_apply(&self) -> Result<(), dbus::Error>;
    fn gtks(&self) -> Result<Vec<Vec<u8>>, dbus::Error>;
    fn gaks(&self) -> Result<Vec<Vec<u8>>, dbus::Error>;
    fn nodes(&self) -> Result<Vec<(Vec<u8>, arg::PropMap)>, dbus::Error>;
    fn hw_address(&self) -> Result<Vec<u8>, dbus::Error>;
    fn wisun_network_name(&self) -> Result<String, dbus::Error>;
    fn wisun_size(&self) -> Result<String, dbus::Error>;
    fn wisun_domain(&self) -> Result<String, dbus::Error>;
    fn wisun_mode(&self) -> Result<u32, dbus::Error>;
    fn wisun_class(&self) -> Result<u32, dbus::Error>;
    fn wisun_pan_id(&self) -> Result<u16, dbus::Error>;
}

impl<'a, T: blocking::BlockingSender, C: ::std::ops::Deref<Target=T>> ComSilabsWisunBorderRouter for blocking::Proxy<'a, C> {

    fn debug_ping(&self, arg0: &str) -> Result<(), dbus::Error> {
        self.method_call("com.silabs.Wisun.BorderRouter", "DebugPing", (arg0, ))
    }

    fn add_root_certificate(&self, arg0: &str) -> Result<(), dbus::Error> {
        self.method_call("com.silabs.Wisun.BorderRouter", "AddRootCertificate", (arg0, ))
    }

    fn remove_root_certificate(&self, arg0: &str) -> Result<(), dbus::Error> {
        self.method_call("com.silabs.Wisun.BorderRouter", "RemoveRootCertificate", (arg0, ))
    }

    fn revoke_node(&self, arg0: Vec<u8>) -> Result<(), dbus::Error> {
        self.method_call("com.silabs.Wisun.BorderRouter", "RevokeNode", (arg0, ))
    }

    fn revoke_apply(&self) -> Result<(), dbus::Error> {
        self.method_call("com.silabs.Wisun.BorderRouter", "RevokeApply", ())
    }

    fn gtks(&self) -> Result<Vec<Vec<u8>>, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "Gtks")
    }

    fn gaks(&self) -> Result<Vec<Vec<u8>>, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "Gaks")
    }

    fn nodes(&self) -> Result<Vec<(Vec<u8>, arg::PropMap)>, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "Nodes")
    }

    fn hw_address(&self) -> Result<Vec<u8>, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "HwAddress")
    }

    fn wisun_network_name(&self) -> Result<String, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "WisunNetworkName")
    }

    fn wisun_size(&self) -> Result<String, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "WisunSize")
    }

    fn wisun_domain(&self) -> Result<String, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "WisunDomain")
    }

    fn wisun_mode(&self) -> Result<u32, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "WisunMode")
    }

    fn wisun_class(&self) -> Result<u32, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "WisunClass")
    }

    fn wisun_pan_id(&self) -> Result<u16, dbus::Error> {
        <Self as blocking::stdintf::org_freedesktop_dbus::Properties>::get(&self, "com.silabs.Wisun.BorderRouter", "WisunPanId")
    }
}
