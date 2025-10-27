document.addEventListener('DOMContentLoaded', () => {
    const authContainer = document.getElementById('auth-container');
    const chargingContainer = document.getElementById('charging-container');
    const loginBtn = document.getElementById('login-btn');
    const registerBtn = document.getElementById('register-btn');
    const logoutBtn = document.getElementById('logout-btn');
    const stationsList = document.getElementById('stations-list');
    const outletsList = document.getElementById('outlets-list');

    // New tab elements
    const loginTabBtn = document.getElementById('login-tab-btn');
    const registerTabBtn = document.getElementById('register-tab-btn');
    const loginForm = document.getElementById('login-form');
    const registerForm = document.getElementById('register-form');

    const API_URL = 'http://localhost:8080';
    let token = localStorage.getItem('token');

    const showChargingView = () => {
        authContainer.style.display = 'none';
        chargingContainer.style.display = 'block';
        fetchStations();
        fetchOutlets();
    };

    const showAuthView = () => {
        authContainer.style.display = 'block';
        chargingContainer.style.display = 'none';
        localStorage.removeItem('token');
        token = null;
        // Default to login tab
        loginTabBtn.click();
    };

    // Tab switching logic
    loginTabBtn.addEventListener('click', () => {
        loginForm.style.display = 'block';
        registerForm.style.display = 'none';
        loginTabBtn.classList.add('active');
        registerTabBtn.classList.remove('active');
    });

    registerTabBtn.addEventListener('click', () => {
        loginForm.style.display = 'none';
        registerForm.style.display = 'block';
        registerTabBtn.classList.add('active');
        loginTabBtn.classList.remove('active');
    });

    if (token) {
        showChargingView();
    } else {
        showAuthView();
    }

    loginBtn.addEventListener('click', async () => {
        const username = document.getElementById('username').value;
        const password = document.getElementById('password').value;

        if (!username || !password) {
            alert('Please enter username and password.');
            return;
        }

        const response = await fetch(`${API_URL}/login/login`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username, password })
        });

        if (response.ok) {
            const data = await response.json();
            token = data.token;
            localStorage.setItem('token', token);
            showChargingView();
        } else {
            alert('Login failed');
        }
    });

    registerBtn.addEventListener('click', async () => {
        const name = document.getElementById('reg-name').value;
        const email = document.getElementById('reg-email').value;
        const password = document.getElementById('reg-password').value;

        if (!name || !email || !password) {
            alert('Please fill out all fields.');
            return;
        }

        const response = await fetch(`${API_URL}/login/register`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name, email, password })
        });

        if (response.ok) {
            alert('Registration successful! Please login.');
            loginTabBtn.click(); // Switch to login tab
        } else {
            alert('Registration failed');
        }
    });

    logoutBtn.addEventListener('click', () => {
        showAuthView();
    });

    async function fetchStations() {
        if (!token) return;
        const response = await fetch(`${API_URL}/api/stations`, {
             headers: { 'Authorization': `Bearer ${token}` }
        });
        const data = await response.json();
        stationsList.innerHTML = '<h3>Available Stations</h3>';
        data.stations.forEach(station => {
            const stationDiv = document.createElement('div');
            stationDiv.className = 'station';
            stationDiv.textContent = `${station.name} - ${station.location}`;
            stationsList.appendChild(stationDiv);
        });
    }

    async function fetchOutlets() {
        if (!token) return;
        const response = await fetch(`${API_URL}/api/outlets`, {
            headers: { 'Authorization': `Bearer ${token}` }
        });
        const data = await response.json();
        outletsList.innerHTML = '<h3>Available Outlets</h3>';
        data.outlets.forEach(outlet => {
            const outletDiv = document.createElement('div');
            outletDiv.className = 'outlet';
            outletDiv.innerHTML = `
                <span>${outlet.name} (Station ${outlet.station_id}) - <strong>${outlet.status}</strong></span>
                <div>
                    <button class="initiate-btn" data-id="${outlet.id}" ${outlet.status !== 'available' ? 'disabled' : ''}>Start</button>
                    <button class="terminate-btn" data-id="${outlet.id}" ${outlet.status === 'available' ? 'disabled' : ''}>Stop</button>
                </div>
            `;
            outletsList.appendChild(outletDiv);
        });
    }

    outletsList.addEventListener('click', async (e) => {
        if (!token) return;

        const target = e.target;
        const outletId = target.dataset.id;

        if (target.classList.contains('initiate-btn')) {
            const response = await fetch(`${API_URL}/api/charge/initiate`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${token}`
                },
                body: JSON.stringify({ outlet_id: parseInt(outletId) })
            });
            if (response.ok) {
                fetchOutlets();
            } else {
                alert('Failed to initiate charge');
            }
        }

        if (target.classList.contains('terminate-btn')) {
            const response = await fetch(`${API_URL}/api/charge/terminate`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${token}`
                },
                body: JSON.stringify({ outlet_id: parseInt(outletId) })
            });
            if (response.ok) {
                fetchOutlets();
            } else {
                alert('Failed to terminate charge');
            }
        }
    });
});