import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart'; // Import FlutterMap package
import 'package:flutter_mapbox_navigation/flutter_mapbox_navigation.dart';
import "package:latlong/latlong.dart" as latLng;

class AdminMap extends StatefulWidget {
  const AdminMap({Key? key}) : super(key: key);

  @override
  _AdminMapState createState() => _AdminMapState();
}

class _AdminMapState extends State<AdminMap> {
  late MapboxNavigation _mapboxNavigation;

  @override
  void initState() {
    super.initState();
    _mapboxNavigation = MapboxNavigation(onRouteEvent: (event) {
      // Handle route events here
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: FlutterMap(
        options: MapOptions(
          center: LatLng(37.42796133580664, -122.885749655962),
          zoom: 14.4746,
        ),
        layers: [
          TileLayerOptions(
            urlTemplate:
            'https://api.mapbox.com/styles/v1/daklit/clecuoi4z000801o9a9xezgk4/wmts?access_token=pk.eyJ1IjoiZGFrbGl0IiwiYSI6ImNsZWNxanJhaDAwMjIzcGt4cjF3MmZ4czgifQ.db8Az0uKza9pFbTrISFKEw',
            additionalOptions: {
              'accessToken': 'pk.eyJ1IjoiZGFrbGl0IiwiYSI6ImNsZWNxanJhaDAwMjIzcGt4cjF3MmZ4czgifQ.db8Az0uKza9pFbTrISFKEw',
              'id': 'mapbox/streets-v11', // You can change the map style here
            },
          ),
        ],
      ),
    );
  }

  @override
  void dispose() {
    _mapboxNavigation.finish();
    super.dispose();
  }
}
