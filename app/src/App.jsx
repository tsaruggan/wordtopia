import { useState } from 'react';
import SearchInput from './SearchInput';

function App() {
  return (
    <>
      <div className='page'>
        <div className='main'>
          <div className='header'>
            <h1>Wordology</h1>
          </div>
          <SearchInput />
          
        </div>
      </div>
    </>
  );
}

export default App;
