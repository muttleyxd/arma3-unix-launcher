{ lib, stdenvNoCC, fetchurl, libarchive }:

# Steamworks SDK as repackaged for CI by julianxhokaxhiu/SteamworksSDKCI.
# The macOS libsteam_api.dylib is a universal binary that includes arm64.
stdenvNoCC.mkDerivation (finalAttrs: {
  pname = "steamworks-sdk";
  version = "1.53";

  src = fetchurl {
    url = "https://github.com/julianxhokaxhiu/SteamworksSDKCI/releases/download/${finalAttrs.version}/SteamworksSDK-v${finalAttrs.version}.0_x64.zip";
    hash = "sha256-6PQGaPsaxBg/MHVWw2ynYW6LaNSrE9Rd9Q9ZLKFGPFA=";
  };

  # The archive stores paths with backslashes, which unzip rejects; bsdtar converts them
  nativeBuildInputs = [ libarchive ];
  dontUnpack = true;

  installPhase = ''
    runHook preInstall
    mkdir -p $out
    bsdtar -xf $src -C $out
    runHook postInstall
  '';

  meta = {
    description = "Valve Steamworks SDK headers and redistributable libsteam_api";
    homepage = "https://partner.steamgames.com/doc/sdk";
    license = lib.licenses.unfree;
    platforms = [ "aarch64-darwin" ];
  };
})
