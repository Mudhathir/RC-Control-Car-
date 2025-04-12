import CoreBluetooth
import SwiftUI
import Combine

// This class manages BLE scanning, connecting, and sending commands
class BluetoothManager: NSObject, ObservableObject {
    // Create a central manager
    var centralManager: CBCentralManager!
    // Holds the connected peripheral (your RC car)
    @Published var rcCarPeripheral: CBPeripheral?
    // A published property to update connection status on UI
    @Published var isConnected: Bool = false
    // The CBCharacteristic used to send commands (replace with your RC car’s characteristic UUID)
    var commandCharacteristic: CBCharacteristic?
    
    // Define your RC car’s service and characteristic UUIDs (replace with your values)
    let rcCarServiceUUID = CBUUID(string: "YOUR-RC-CAR-SERVICE-UUID")
    let commandCharacteristicUUID = CBUUID(string: "YOUR-COMMAND-CHARACTERISTIC-UUID")
    
    override init() {
        super.init()
        // Initialize central manager on the main queue
        centralManager = CBCentralManager(delegate: self, queue: DispatchQueue.main)
    }
    
    // MARK: - Public Functions
    
    /// Start scanning for the RC car peripheral.
    func startScanning() {
        // Scan for peripherals advertising the RC car service.
        centralManager.scanForPeripherals(withServices: [rcCarServiceUUID], options: nil)
        print("Scanning started...")
    }
    
    /// Send a command (as Data) to the RC car.
    func sendCommand(_ command: Data) {
        guard let peripheral = rcCarPeripheral,
              let characteristic = commandCharacteristic else {
            print("Not connected or characteristic not found")
            return
        }
        // Write command to the characteristic (without response can be used if supported)
        peripheral.writeValue(command, for: characteristic, type: .withoutResponse)
        print("Sent command: \(command)")
    }
    
    /// Disconnect from the RC car
    func disconnect() {
        if let peripheral = rcCarPeripheral {
            centralManager.cancelPeripheralConnection(peripheral)
        }
    }
}

// MARK: - CBCentralManagerDelegate
extension BluetoothManager: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        // Check if Bluetooth is powered on
        if central.state == .poweredOn {
            print("Bluetooth is powered on")
            startScanning()
        } else {
            print("Bluetooth state is: \(central.state.rawValue)")
        }
    }
    
    // Discovered peripheral callback
    func centralManager(_ central: CBCentralManager,
                        didDiscover peripheral: CBPeripheral,
                        advertisementData: [String : Any],
                        rssi RSSI: NSNumber) {
        print("Discovered \(peripheral.name ?? "Unknown")")
        // Save and connect if it matches the RC car’s name (or filter via UUID)
        if let name = peripheral.name, name.contains("RC Car") {
            rcCarPeripheral = peripheral
            peripheral.delegate = self
            centralManager.stopScan()
            centralManager.connect(peripheral, options: nil)
        }
    }
    
    // Successful connection callback
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected to RC Car!")
        isConnected = true
        // Discover services on the peripheral
        peripheral.discoverServices([rcCarServiceUUID])
    }
    
    // Connection error handling
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        print("Failed to connect: \(error?.localizedDescription ?? "Unknown error")")
        isConnected = false
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Disconnected from RC Car")
        isConnected = false
        // Optionally: restart scanning
        startScanning()
    }
}

// MARK: - CBPeripheralDelegate
extension BluetoothManager: CBPeripheralDelegate {
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let error = error {
            print("Service discovery error: \(error.localizedDescription)")
            return
        }
        guard let services = peripheral.services else { return }
        for service in services {
            if service.uuid == rcCarServiceUUID {
                // Discover the characteristic used for sending commands
                peripheral.discoverCharacteristics([commandCharacteristicUUID], for: service)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral,
                    didDiscoverCharacteristicsFor service: CBService,
                    error: Error?) {
        if let error = error {
            print("Characteristic discovery error: \(error.localizedDescription)")
            return
        }
        guard let characteristics = service.characteristics else { return }
        for char in characteristics {
            if char.uuid == commandCharacteristicUUID {
                commandCharacteristic = char
                print("Found command characteristic")
            }
        }
    }
    
    // Optionally handle data received from the RC car if needed.
    func peripheral(_ peripheral: CBPeripheral,
                    didUpdateValueFor characteristic: CBCharacteristic,
                    error: Error?) {
        if let error = error {
            print("Error updating characteristic: \(error.localizedDescription)")
            return
        }
        if let data = characteristic.value {
            print("Received data: \(data)")
        }
    }
}
