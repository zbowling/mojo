// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:io';
import 'dart:async';

import 'package:archive/archive.dart';
import 'package:args/args.dart';
import 'package:yaml/yaml.dart';

const String kSnapshotKey = 'snapshot_blob.bin';
const List<String> kDensities = const ['drawable-xxhdpi'];
const List<String> kThemes = const ['white', 'black', 'grey600'];
const List<int> kSizes = const [24];

class MaterialAsset {
  final String name;
  final String density;
  final String theme;
  final int size;

  MaterialAsset(Map descriptor)
    : name = descriptor['name'],
      density = descriptor['density'],
      theme = descriptor['theme'],
      size = descriptor['size'];

  String get key {
    List<String> parts = name.split('/');
    String category = parts[0];
    String subtype = parts[1];
    return '$category/$density/ic_${subtype}_${theme}_${size}dp.png';
  }
}

List generateValues(Map assetDescriptor, String key, List defaults) {
  if (assetDescriptor.containsKey(key))
    return [assetDescriptor[key]];
  return defaults;
}

Iterable<MaterialAsset> generateMaterialAssets(Map assetDescriptor) sync* {
  Map currentAssetDescriptor = new Map.from(assetDescriptor);
  for (String density in generateValues(assetDescriptor, 'density', kDensities)) {
    currentAssetDescriptor['density'] = density;
    for (String theme in generateValues(assetDescriptor, 'theme', kThemes)) {
      currentAssetDescriptor['theme'] = theme;
      for (String size in generateValues(assetDescriptor, 'size', kSizes)) {
        currentAssetDescriptor['size'] = size;
        yield new MaterialAsset(currentAssetDescriptor);
      }
    }
  }
}

Iterable<MaterialAsset> parseMaterialAssets(Map manifestDescriptor) sync* {
  for (Map assetDescriptor in manifestDescriptor['material-design-icons']) {
    for (MaterialAsset asset in generateMaterialAssets(assetDescriptor)) {
      yield asset;
    }
  }
}

Future loadManifest(String manifestPath) async {
  String manifestDescriptor = await new File(manifestPath).readAsString();
  return loadYaml(manifestDescriptor);
}

Future<ArchiveFile> createFile(MaterialAsset asset, String assetBase) async {
  File file = new File('${assetBase}/${asset.key}');
  List<int> content = await file.readAsBytes();
  return new ArchiveFile.noCompress(asset.key, content.length, content);
}

Future<ArchiveFile> createSnapshotFile(String snapshotPath) async {
  File file = new File(snapshotPath);
  List<int> content = await file.readAsBytes();
  return new ArchiveFile(kSnapshotKey, content.length, content);
}

main(List<String> argv) async {
  ArgParser parser = new ArgParser();
  parser.addFlag('help', abbr: 'h', negatable: false);
  parser.addOption('asset-base');
  parser.addOption('snapshot');
  parser.addOption('output-file', abbr: 'o');

  ArgResults args = parser.parse(argv);
  if (args['help']) {
    print(parser.usage);
    return;
  }

  String manifestPath = args.rest.first;

  Map manifestDescriptor = await loadManifest(manifestPath);
  Iterable<MaterialAsset> materialAssets = parseMaterialAssets(manifestDescriptor);

  Archive archive = new Archive();

  String snapshot = args['snapshot'];
  if (snapshot != null)
    archive.addFile(await createSnapshotFile(snapshot));

  for (MaterialAsset asset in materialAssets)
    archive.addFile(await createFile(asset, args['asset-base']));

  File outputFile = new File(args['output-file']);
  await outputFile.writeAsBytes(new ZipEncoder().encode(archive));
}