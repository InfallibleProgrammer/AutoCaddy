//
//  ViewController.swift
//  AutoCaddy
//
//  Created by Justin Stokes on 1/22/23.
//

import UIKit
import CoreLocation
import CoreBluetooth

//class ViewController: UIViewController, CLLocationManagerDelegate
class ViewController: UIViewController, CLLocationManagerDelegate {
    
    // labels
    @IBOutlet weak var statusLabel: UILabel!
    @IBOutlet weak var distanceLabel: UILabel!
    @IBOutlet weak var bearingLabel: UILabel!
    @IBOutlet weak var distanceMeasurementLabel: UILabel!
    @IBOutlet weak var bearingMeasurementLabel: UILabel!
    
    // buttons
    @IBOutlet var startButton: UIButton!
    @IBOutlet var stopButton: UIButton!
    @IBOutlet weak var distanceAndBearingButton: UIButton!
    
    // sliders
    @IBOutlet weak var bearingSlider: UISlider!
    @IBOutlet weak var distanceSlider: UISlider!
    
    var locationManager: CLLocationManager = CLLocationManager()
    var bluetoothManager: CBCentralManager!
    var bluetoothPeripheral: CBPeripheral!
    var tx_characteristic: CBCharacteristic?
    
    var bluetooth_is_connected = false
    var transmit_location = false
    var characteristic_found = false
    
    var tx_UUID = CBUUID(string: "FFE1")
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        bluetoothManager = CBCentralManager.init(delegate: self, queue: nil)
        locationManager.desiredAccuracy = kCLLocationAccuracyBestForNavigation
        locationManager.activityType = CLActivityType.otherNavigation
        if(transmit_location) {
            startButton.setTitle("Transmitting Location...", for: .normal)
        } else {
            startButton.setTitle("Transmit Location", for: .normal)
        }
        if(bluetooth_is_connected) {
            statusLabel.text = "Connected to AutoCaddy!"
        } else {
            
            statusLabel.text = "Searching for AutoCaddy..."
        }
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBestForNavigation
        locationManager.requestWhenInUseAuthorization()
        locationManager.startUpdatingLocation()
        distanceSlider.isEnabled = true
        bearingSlider.isEnabled = true
        distanceLabel.text = "Distance"
        distanceMeasurementLabel.text = String(distanceSlider.value)
        bearingMeasurementLabel.text = String(bearingSlider.value)
        bearingLabel.text = "Bearing"
    }
    
    func sendOverBLE(withValue value: Data) {
        bluetoothPeripheral.writeValue(value, for: tx_characteristic!, type: .withResponse)
    }
    
    // retrieves GPS coordinates through CLLocationManager
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let first = locations.first else {
            return
        }
        if(bluetooth_is_connected && transmit_location && characteristic_found) {
            let longitude = String(first.coordinate.longitude) + ","
            let longitude_string = "Lo:"
            guard let data = longitude_string.data(using: String.Encoding.utf8) else { return }
            sendOverBLE(withValue: data)
            guard let data = longitude.data(using: String.Encoding.utf8) else { return }
            sendOverBLE(withValue: data)
            let latitude = String(first.coordinate.latitude) + ","
            let latitude_string = "La:"
            guard let data = latitude_string.data(using: String.Encoding.utf8) else { return }
            sendOverBLE(withValue: data)
            guard let data = latitude.data(using: String.Encoding.utf8) else { return }
            sendOverBLE(withValue: data)
            print("latitude: " + String(first.coordinate.latitude))
            print("longitude: " + String(first.coordinate.longitude))
        }
    }
    
    // action to perform when 'start button` is tapped
    @IBAction func startButtonTapped(_ sender: Any) {
        if(bluetooth_is_connected) {
            startButton.setTitle("Transmitting Location...", for: .normal)
            transmit_location = true
        }
    }
    
    // action to perform when 'stop button is tapped
    @IBAction func stopButtonTapped(_ sender: Any) {
        startButton.setTitle("Transmit Location", for: .normal)
        transmit_location = false
        let stopString = "S:0,"
        guard let command = stopString.data(using: String.Encoding.utf8) else { return }
        sendOverBLE(withValue: command)
    }
    
    // action to perform when 'distanceAndBearing' button is tapped
    
    @IBAction func distanceAndBearingButtonTapped(_ sender: Any) {
        transmit_location = false
        startButton.setTitle("Transmit Location", for: .normal)
        let distanceValue = "D:" + String(distanceSlider.value) + ","
        let bearingValue = "B:" + String(bearingSlider.value) + ","
        guard let distanceData = distanceValue.data(using: String.Encoding.utf8) else { return }
        sendOverBLE(withValue: distanceData)
        guard let bearingData = bearingValue.data(using: String.Encoding.utf8) else { return }
        sendOverBLE(withValue: bearingData)
    }
    
    // action performed when distanceSlider is changed
    @IBAction func distanceSliderChanged(_ sender: Any) {
        distanceMeasurementLabel.text = String(distanceSlider.value)
    }
    
    //action performed when bearingSlider is changed
    @IBAction func bearingSliderChanged(_ sender: Any) {
        bearingMeasurementLabel.text = String(bearingSlider.value)
    }
    
}

// extension to handle Bluetooth states
extension ViewController: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .unknown:
            print("unknown state")
        case .resetting:
            print("resetting state")
        case .unsupported:
            print("unsupported state")
        case .unauthorized:
            print("unauthorized state")
        case .poweredOff:
            print("powered off state")
        case .poweredOn:
            bluetoothManager.scanForPeripherals(withServices: nil)
        @unknown default:
            print("default state")
        }
    }
    
    // search for "auto_caddy" peripheral and connect
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if peripheral.name == "auto_caddy" {
            statusLabel.text = "Connecting to AutoCaddy..."
            bluetoothPeripheral = peripheral
            bluetoothPeripheral.delegate = self
            bluetoothManager.stopScan()
            bluetoothManager.connect(bluetoothPeripheral)
        }
    }

    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        bluetooth_is_connected = true
        statusLabel.text = "Connected to AutoCaddy!"
        bluetoothPeripheral.discoverServices(nil)
    }
}

// extension to handle discovering peripheral services and characteristics
extension ViewController : CBPeripheralDelegate {
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        
        for service in services {
            bluetoothPeripheral.discoverCharacteristics(nil, for: service)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        
        for characteristic in characteristics {
            if characteristic.uuid == tx_UUID {
                characteristic_found = true
                tx_characteristic = characteristic
                print("Ready to transmit")
            }
        }
    }
}
