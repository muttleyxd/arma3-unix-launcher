{ writeShellApplication, coreutils, arma3-unix-launcher }:

# Runs outside the build sandbox via `nix run .#dmg -- [output.dmg]`: Apple's codesign and hdiutil
# are not available inside it. The sandbox's ad-hoc signatures cannot seal bundle resources, so
# the whole bundle is re-signed (still ad-hoc) before it is packed.
writeShellApplication {
  name = "make-dmg";
  runtimeInputs = [ coreutils ];
  text = ''
    app_name="Arma 3 Unix Launcher"
    output="$(realpath -m "''${1:-$app_name.dmg}")"

    staging="$(mktemp -d -t a3ul-dmg.XXXXXX)"
    trap 'rm -rf "$staging"' EXIT

    cp -R "${arma3-unix-launcher}/Applications/$app_name.app" "$staging/"
    chmod -R u+w "$staging"
    ln -s /Applications "$staging/Applications"

    /usr/bin/codesign --force --deep --sign - "$staging/$app_name.app"
    /usr/bin/codesign --verify --deep --strict "$staging/$app_name.app"

    rm -f "$output"
    /usr/bin/hdiutil create -quiet -volname "$app_name" -srcfolder "$staging" \
                            -fs HFS+ -format UDZO "$output"
    echo "Created $output"
  '';
}
