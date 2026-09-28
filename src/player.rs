use anyhow::{bail, Context, Result};

pub struct Player {
    lib: Option<libloading::Library>,
    instance: *mut std::ffi::c_void,
    media: *mut std::ffi::c_void,
    player: *mut std::ffi::c_void,
}

impl Player {
    pub fn new() -> Self {
        Self {
            lib: None,
            instance: std::ptr::null_mut(),
            media: std::ptr::null_mut(),
            player: std::ptr::null_mut(),
        }
    }

    pub fn play(&mut self, url: &str) -> Result<()> {
        self.stop()?;
        let library = load_libvlc().context("loading libVLC (install VLC)")?;
        let url = std::ffi::CString::new(url).context("stream URL contains a NUL byte")?;
        unsafe {
            let new_instance = symbol::<
                unsafe extern "C" fn(i32, *const *const i8) -> *mut std::ffi::c_void,
            >(&library, b"libvlc_new\0")?;
            let instance = new_instance(0, std::ptr::null());
            if instance.is_null() {
                bail!("libVLC could not create an instance");
            }
            let new_media = symbol::<
                unsafe extern "C" fn(*mut std::ffi::c_void, *const i8) -> *mut std::ffi::c_void,
            >(&library, b"libvlc_media_new_location\0")?;
            let media = new_media(instance, url.as_ptr());
            if media.is_null() {
                release_instance(&library, instance);
                bail!("libVLC could not create media for the stream");
            }
            let new_player = symbol::<
                unsafe extern "C" fn(*mut std::ffi::c_void) -> *mut std::ffi::c_void,
            >(&library, b"libvlc_media_player_new\0")?;
            let player = new_player(instance);
            if player.is_null() {
                release_media(&library, media);
                release_instance(&library, instance);
                bail!("libVLC could not create a media player");
            }
            let set_media = symbol::<
                unsafe extern "C" fn(*mut std::ffi::c_void, *mut std::ffi::c_void),
            >(&library, b"libvlc_media_player_set_media\0")?;
            set_media(player, media);
            let play = symbol::<unsafe extern "C" fn(*mut std::ffi::c_void) -> i32>(
                &library,
                b"libvlc_media_player_play\0",
            )?;
            if play(player) != 0 {
                release_player(&library, player);
                release_media(&library, media);
                release_instance(&library, instance);
                bail!("libVLC could not start playback");
            }
            self.instance = instance;
            self.media = media;
            self.player = player;
        }
        self.lib = Some(library);
        Ok(())
    }

    pub fn stop(&mut self) -> Result<()> {
        if let Some(library) = self.lib.take() {
            unsafe {
                if !self.player.is_null() {
                    let stop = symbol::<unsafe extern "C" fn(*mut std::ffi::c_void)>(
                        &library,
                        b"libvlc_media_player_stop\0",
                    )?;
                    stop(self.player);
                    release_player(&library, self.player);
                }
                if !self.media.is_null() {
                    release_media(&library, self.media);
                }
                if !self.instance.is_null() {
                    release_instance(&library, self.instance);
                }
            }
        }
        self.player = std::ptr::null_mut();
        self.media = std::ptr::null_mut();
        self.instance = std::ptr::null_mut();
        Ok(())
    }
}

impl Drop for Player {
    fn drop(&mut self) {
        let _ = self.stop();
    }
}

fn load_libvlc() -> Result<libloading::Library> {
    for name in ["libvlc.so.5", "libvlc.so", "libvlc.dylib", "libvlc.dll"] {
        if let Ok(library) = unsafe { libloading::Library::new(name) } {
            return Ok(library);
        }
    }
    bail!("libVLC shared library was not found")
}

unsafe fn symbol<'a, T>(
    library: &'a libloading::Library,
    name: &[u8],
) -> Result<libloading::Symbol<'a, T>> {
    Ok(library.get(name)?)
}

unsafe fn release_player(library: &libloading::Library, player: *mut std::ffi::c_void) {
    if let Ok(release) = symbol::<unsafe extern "C" fn(*mut std::ffi::c_void)>(
        library,
        b"libvlc_media_player_release\0",
    ) {
        release(player);
    }
}

unsafe fn release_media(library: &libloading::Library, media: *mut std::ffi::c_void) {
    if let Ok(release) =
        symbol::<unsafe extern "C" fn(*mut std::ffi::c_void)>(library, b"libvlc_media_release\0")
    {
        release(media);
    }
}

unsafe fn release_instance(library: &libloading::Library, instance: *mut std::ffi::c_void) {
    if let Ok(release) =
        symbol::<unsafe extern "C" fn(*mut std::ffi::c_void)>(library, b"libvlc_release\0")
    {
        release(instance);
    }
}
