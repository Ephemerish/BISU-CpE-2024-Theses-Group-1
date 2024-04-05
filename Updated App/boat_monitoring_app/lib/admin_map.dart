import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:latlong2/latlong.dart';

class AdminMap extends StatelessWidget {
  const AdminMap({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Stack(
        children: [
          FlutterMap(
            options: MapOptions(
              initialCenter:
              LatLng(9.6488, 123.8585), // Centered in the waters of Bohol
              initialZoom: 11, // Increased zoom level
            ),
            children: [
              TileLayer(
                urlTemplate:
                'https://api.mapbox.com/styles/v1/daklit/clujs0qwm00r101q50jw1e3pw/tiles/256/{z}/{x}/{y}@2x?access_token=pk.eyJ1IjoiZGFrbGl0IiwiYSI6ImNsZWNxanJhaDAwMjIzcGt4cjF3MmZ4czgifQ.db8Az0uKza9pFbTrISFKEw',
                additionalOptions: {
                  'accessToken':
                  'pk.eyJ1IjoiZGFrbGl0IiwiYSI6ImNsZWNxanJhaDAwMjIzcGt4cjF3MmZ4czgifQ.db8Az0uKza9pFbTrISFKEw',
                  'id': 'mapbox.satellite',
                },
              ),
              MarkerLayer(
                markers: [
                  Marker(
                    point: LatLng(
                        10.0, 123.8), // Adjusted coordinates for Marker 1
                    width: 40.0,
                    height: 40.0,
                    // Use the 'child' parameter instead of 'builder'
                    child: GestureDetector(
                      onTap: () {
                        // Handle marker tap
                        _showMarkerDescription(context,
                            "Boat Number: 123\nOwner: John Doe\nLocation: (10.0, 123.8)");
                      },
                      child: Image.asset(
                        'assets/boat_icon_1.png',
                        width: 40.0,
                        height: 40.0,
                      ),
                    ),
                  ),
                  Marker(
                    point: LatLng(
                      9.9273, // Adjusted latitude for GW-001
                      123.9164, // Adjusted longitude for GW-001
                    ),
                    width: 40.0,
                    height: 40.0,
                    child: GestureDetector(
                      onTap: () {
                        // Handle marker tap
                        _showMarkerDescription(context, "GW - 001");
                      },
                      child: Image.asset(
                        'assets/gateway_node.png',
                        width: 40.0,
                        height: 40.0,
                      ),
                    ),
                  ),
                  Marker(
                    point:
                    LatLng(10, 124.0), // Adjusted coordinates for Marker 2
                    width: 40.0,
                    height: 40.0,
                    // Use the 'child' parameter instead of 'builder'
                    child: GestureDetector(
                      onTap: () {
                        // Handle marker tap
                        _showMarkerDescription(context,
                            "Boat Number: 456\nOwner: Jane Doe\nLocation: (9.8, 124.0)");
                      },
                      child: Image.asset(
                        'assets/boat_icon_2.png',
                        width: 40.0,
                        height: 40.0,
                      ),
                    ),
                  ),
                ],
              ),
            ],
          ),
        ],
      ),
    );
  }

  void _showMarkerDescription(BuildContext context, String description) {
    showDialog(
      context: context,
      barrierDismissible: true, // Allow dismissing dialog on tap outside
      builder: (context) => Stack(
        children: [
          GestureDetector(
            onTap: () => Navigator.pop(context), // Dismiss dialog on tap
            child: Container(
              color: Colors.transparent, // Make the container transparent
              width: MediaQuery.of(context).size.width,
              height: MediaQuery.of(context).size.height,
            ),
          ),
          Positioned(
            bottom: 20.0, // Adjust this value as needed
            left: 0,
            right: 0,
            child: AlertDialog(
              title: Text("  "),
              content: Text(description),
            ),
          ),
        ],
      ),
    );
  }
}


